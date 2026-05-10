/**
 * @file    WebOtaUpdate.cpp
 * @brief   Implementation of the HTTP OTA firmware update service.
 *
 * @details Registers HTTP routes on an ESP32 WebServer instance and handles
 *          multipart firmware binary uploads using the Arduino Update library.
 *          The entire implementation is conditionally compiled behind
 *          WEB_OTA_ENABLED so it can be stripped from production builds with
 *          a single build flag.
 *
 *          Route map:
 *          - GET  /        → redirects to /update
 *          - GET  /update  → serves the upload HTML form
 *          - POST /update  → receives the .bin file and flashes it; reboots on success
 *          - *             → 404 for any other path
 *
 *          All routes require HTTP Basic Authentication (WEB_OTA_HTTP_USER /
 *          WEB_OTA_HTTP_PASS).
 */

#include "WebOtaUpdate.h"

#if WEB_OTA_ENABLED

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

namespace WebOtaUpdate {

/** @brief Singleton HTTP server instance bound to WEB_OTA_PORT. */
static WebServer server(WEB_OTA_PORT);

/** @brief True once begin() has successfully started the server. */
static bool g_running = false;

/** @brief True while a firmware binary is being received and written. */
static bool g_updating = false;

/**
 * @brief Inline HTML served at GET /update.
 *
 * Stored in PROGMEM to conserve heap. Contains a minimal Bootstrap-free
 * form that POSTs a .bin file to /update as multipart/form-data.
 */
static const char UPDATE_PAGE_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>OpenHornet OTA Update</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 2rem; max-width: 680px; }
    h1 { margin-bottom: 0.5rem; }
    .card { border: 1px solid #ddd; padding: 1rem; border-radius: 8px; }
    .muted { color: #666; font-size: 0.95rem; }
    button { margin-top: 0.8rem; padding: 0.5rem 1rem; }
    input[type=file] { margin-top: 0.5rem; }
  </style>
</head>
<body>
  <h1>OpenHornet Firmware Update</h1>
  <p class="muted">Upload a PlatformIO firmware binary (.bin) to install OTA update.</p>
  <div class="card">
    <form method="POST" action="/update" enctype="multipart/form-data">
      <label>Select firmware (.bin):</label><br/>
      <input type="file" name="firmware" accept=".bin,application/octet-stream" required><br/>
      <button type="submit">Upload and Install</button>
    </form>
  </div>
</body>
</html>
)HTML";

/**
 * @brief  Enforce HTTP Basic Authentication on the current request.
 *
 * @details Sends a 401 challenge if the request does not carry valid credentials.
 *          All route handlers call this first and return early on failure.
 *
 * @return @c true  if the request is authenticated and handling may continue.
 * @return @c false if a 401 response was sent and the handler must return.
 */
static bool ensureAuth() {
  if (!server.authenticate(WEB_OTA_HTTP_USER, WEB_OTA_HTTP_PASS)) {
    server.requestAuthentication(BASIC_AUTH, "OpenHornet OTA");
    return false;
  }
  return true;
}

/**
 * @brief  Handle GET / — redirect browser to /update.
 *
 * @details Issues a 302 redirect so that typing the bare IP in a browser
 *          lands directly on the firmware upload page.
 */
static void handleRoot() {
  if (!ensureAuth()) return;
  server.sendHeader("Location", "/update", true);
  server.send(302, "text/plain", "Redirecting to /update");
}

/**
 * @brief  Handle GET /update — serve the firmware upload HTML form.
 *
 * @details Sends UPDATE_PAGE_HTML directly from PROGMEM (send_P) to avoid
 *          copying it into RAM.
 */
static void handleUpdateGet() {
  if (!ensureAuth()) return;
  server.send_P(200, "text/html", UPDATE_PAGE_HTML);
}

/**
 * @brief  Handle POST /update completion — report result and reboot on success.
 *
 * @details Called by the WebServer after handleUpdateUpload() has processed
 *          all upload chunks. Sends a 500 if Update.hasError(), or a 200
 *          and triggers ESP.restart() after a short delay on success.
 */
static void handleUpdatePostDone() {
  if (!ensureAuth()) return;

  if (Update.hasError()) {
    server.send(500, "text/plain", "Update failed. See serial logs.");
    g_updating = false;
    return;
  }

  server.send(200, "text/plain", "Update successful. Rebooting...");
  delay(500);
  ESP.restart();
}

/**
 * @brief  Handle POST /update upload chunks — write binary data to flash.
 *
 * @details Called repeatedly by WebServer for each multipart chunk during
 *          the upload. Drives the Update state machine through START → WRITE
 *          → END. Any write error aborts the update and clears g_updating so
 *          handleUpdatePostDone() can report the failure.
 *
 * @note   UPLOAD_FILE_ABORTED is also handled to cleanly cancel an in-progress
 *         Update session if the client disconnects mid-transfer.
 */
static void handleUpdateUpload() {
  if (!ensureAuth()) return;

  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    g_updating = true;
    Serial.printf("[WebOTA] Upload start: %s\n", upload.filename.c_str());

    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Update.printError(Serial);
      g_updating = false;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (g_updating) {
      size_t written = Update.write(upload.buf, upload.currentSize);
      if (written != upload.currentSize) {
        Update.printError(Serial);
        g_updating = false;
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (g_updating) {
      if (Update.end(true)) {
        Serial.printf("[WebOTA] Upload complete: %u bytes\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    } else {
      Serial.println("[WebOTA] Upload ended with prior error.");
    }
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    Update.abort();
    g_updating = false;
    Serial.println("[WebOTA] Upload aborted.");
  }
}

/**
 * @brief  Handle any unregistered URL — return 404.
 */
static void handleNotFound() {
  if (!ensureAuth()) return;
  server.send(404, "text/plain", "Not found");
}

/**
 * @brief  Start the ESP32 Wi-Fi soft access point as a fallback network.
 *
 * @details Creates an open (WPA2) AP using WEB_OTA_AP_SSID and
 *          WEB_OTA_AP_PASSWORD. Clients connect to this network to access
 *          the OTA page when no external router is available.
 */
static void startAccessPointFallback() {
  WiFi.mode(WIFI_AP);
  bool apOk = WiFi.softAP(WEB_OTA_AP_SSID, WEB_OTA_AP_PASSWORD);
  if (apOk) {
    Serial.printf("[WebOTA] AP mode active. SSID: %s, IP: %s\n",
                  WEB_OTA_AP_SSID, WiFi.softAPIP().toString().c_str());
  } else {
    Serial.println("[WebOTA] Failed to start AP mode.");
  }
}

/**
 * @brief  Attempt to connect to an existing Wi-Fi network in STA mode.
 *
 * @details Blocks for up to 15 seconds waiting for an IP address. Prints
 *          progress dots to Serial during the wait.
 *
 * @param staSsid  SSID to join. Returns false immediately if null or empty.
 * @param staPass  WPA2 password for the network.
 *
 * @return @c true  if connected and an IP was assigned within the timeout.
 * @return @c false if credentials are missing or the connection timed out.
 */
static bool startStation(const char* staSsid, const char* staPass) {
  if (!staSsid || !staPass || strlen(staSsid) == 0) {
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(WEB_OTA_HOSTNAME);
  WiFi.begin(staSsid, staPass);

  Serial.printf("[WebOTA] Connecting to WiFi SSID: %s\n", staSsid);
  const uint32_t timeoutMs = 15000;
  uint32_t startMs = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < timeoutMs) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WebOTA] STA connected. IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
  }

  Serial.println("[WebOTA] STA connect timeout.");
  return false;
}

/**
 * @brief  Start the OTA web update service. See WebOtaUpdate.h for full details.
 *
 * @param staSsid  Wi-Fi SSID to join in STA mode (nullptr triggers AP fallback).
 * @param staPass  Wi-Fi password for STA mode.
 */
void begin(const char* staSsid, const char* staPass) {
  if (g_running) return;

  bool staOk = startStation(staSsid, staPass);
  if (!staOk) {
    startAccessPointFallback();
  }

  server.on("/",       HTTP_GET,  handleRoot);
  server.on("/update", HTTP_GET,  handleUpdateGet);
  server.on("/update", HTTP_POST, handleUpdatePostDone, handleUpdateUpload);
  server.onNotFound(handleNotFound);
  server.begin();

  g_running = true;
  Serial.printf("[WebOTA] HTTP server started on port %d\n", WEB_OTA_PORT);
  Serial.printf("[WebOTA] Update URL: http://%s/update\n", ipAddress().c_str());
}

/**
 * @brief  Process pending HTTP client requests. Call once per loop().
 */
void handle() {
  if (!g_running) return;
  server.handleClient();
}

/**
 * @brief  Query whether the OTA service has been started.
 * @return @c true if begin() completed successfully.
 */
bool isRunning() {
  return g_running;
}

/**
 * @brief  Return the active network IP address.
 *
 * @details Returns the soft-AP IP when in AP mode or when STA is not
 *          connected; otherwise returns the STA-assigned IP.
 *
 * @return Arduino String with the dotted-decimal IP address.
 */
String ipAddress() {
  if (WiFi.getMode() == WIFI_AP || WiFi.status() != WL_CONNECTED) {
    return WiFi.softAPIP().toString();
  }
  return WiFi.localIP().toString();
}

} // namespace WebOtaUpdate

#else  // WEB_OTA_ENABLED == 0

/** @brief Stub implementations compiled when WEB_OTA_ENABLED is 0. */
namespace WebOtaUpdate {
void   begin(const char*, const char*) {}
void   handle()                        {}
bool   isRunning()                     { return false; }
String ipAddress()                     { return String(); }
} // namespace WebOtaUpdate

#endif // WEB_OTA_ENABLED
