/**
 * @file    version.h
 * @brief   Firmware version string for the OpenHornet Up-Front Controller.
 *
 * @details The version string follows the format "F" + three-digit build number.
 *          Increment the numeric suffix on each release build so the version
 *          is visible in OTA logs, boot messages, and Doxygen output.
 */

#ifndef VERSION_H
#define VERSION_H

/**
 * @brief Human-readable firmware version string.
 *
 * Format: "F" followed by a three-digit zero-padded build number.
 * Example: "F100" = firmware build 100.
 * Shown in serial boot output and OTA update page.
 */
#define VERSION_STRING "F100"

#endif // VERSION_H
