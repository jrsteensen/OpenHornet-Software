/**
 * @file    WebOtaUpdate.h
 * @brief   HTTP-based Over-The-Air (OTA) firmware update service for the OpenHornet UFC.
 *
 * @details Provides a lightweight HTTP web server that serves a firmware upload page
 *          and handles multipart binary uploads to flash new firmware via the ESP32
 *          Update subsystem. The service can operate in two network modes:
 *
 *          - **STA mode**: connects to an existing Wi-Fi network using credentials
 *            supplied at runtime via begin(). Preferred when a router is available.
 *          - **AP mode**: falls back to creating its own access point (SSID/password
 *            configurable via compile-time defines) when STA credentials are absent
 *            or the connection times out.
 *
 *          OTA mode is only activated when the COM1 encoder pushbutton (ENC1PB) is
 *          held during power-on. Normal operation is unaffected when OTA is inactive.
 *
 * @note    All compile-time defaults can be overridden via PlatformIO build_flags:
 *          - WEB_OTA_ENABLED      (1/0)
 *          - WEB_OTA_HOSTNAME     (mDNS hostname)
 *          - WEB_OTA_AP_SSID      (fallback AP network name)
 *          - WEB_OTA_AP_PASSWORD  (fallback AP password)
 *          - WEB_OTA_HTTP_USER    (Basic-Auth username for the update page)
 *          - WEB_OTA_HTTP_PASS    (Basic-Auth password for the update page)
 *          - WEB_OTA_PORT         (HTTP server port, default 80)
 */

#pragma once

#include <Arduino.h>

namespace WebOtaUpdate {

/** @brief Set to 0 in build_flags to compile out the entire OTA subsystem. */
#ifndef WEB_OTA_ENABLED
#define WEB_OTA_ENABLED 1
#endif

/** @brief mDNS hostname advertised on the local network in STA mode. */
#ifndef WEB_OTA_HOSTNAME
#define WEB_OTA_HOSTNAME "oh-ufc"
#endif

/** @brief SSID broadcast when the module falls back to AP mode. */
#ifndef WEB_OTA_AP_SSID
#define WEB_OTA_AP_SSID "OH-UFC-OTA"
#endif

/** @brief WPA2 password for the fallback AP. Minimum 8 characters. */
#ifndef WEB_OTA_AP_PASSWORD
#define WEB_OTA_AP_PASSWORD "openhornet"
#endif

/** @brief HTTP Basic-Auth username required to access the update page. */
#ifndef WEB_OTA_HTTP_USER
#define WEB_OTA_HTTP_USER "admin"
#endif

/** @brief HTTP Basic-Auth password required to access the update page. */
#ifndef WEB_OTA_HTTP_PASS
#define WEB_OTA_HTTP_PASS "openhornet"
#endif

/** @brief TCP port the HTTP server listens on. */
#ifndef WEB_OTA_PORT
#define WEB_OTA_PORT 80
#endif

/**
 * @brief  Start the OTA web update service.
 *
 * @details Attempts to connect to the supplied Wi-Fi network in STA mode.
 *          If @p staSsid is null, empty, or the connection times out after
 *          15 seconds, the module falls back to AP mode using WEB_OTA_AP_SSID
 *          and WEB_OTA_AP_PASSWORD. After network setup, registers HTTP routes
 *          and starts the server. Calling begin() more than once is a no-op.
 *
 * @param staSsid  SSID of the Wi-Fi network to join (nullptr → AP fallback).
 * @param staPass  Password for the Wi-Fi network (nullptr → AP fallback).
 */
void begin(const char* staSsid = nullptr, const char* staPass = nullptr);

/**
 * @brief  Process pending HTTP requests.
 *
 * @details Must be called once per loop() iteration while OTA mode is active.
 *          Internally calls WebServer::handleClient(). If the service has not
 *          been started via begin(), this function returns immediately.
 */
void handle();

/**
 * @brief  Query whether the OTA service is running.
 * @return @c true after a successful call to begin(), @c false otherwise.
 */
bool isRunning();

/**
 * @brief  Get the active network IP address as a string.
 *
 * @details Returns the STA IP when connected to a router, or the soft-AP IP
 *          when operating in AP mode.
 *
 * @return Arduino @c String containing the dotted-decimal IP address.
 */
String ipAddress();

} // namespace WebOtaUpdate
