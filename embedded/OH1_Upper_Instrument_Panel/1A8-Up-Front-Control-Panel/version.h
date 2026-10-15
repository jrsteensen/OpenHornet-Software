/**
 * @file    version.h
 * @brief   Firmware version string for the OpenHornet Up-Front Controller (1A8-UFC).
 *
 * @details Semantic version (MAJOR.MINOR.PATCH). Bump MAJOR on a functional rewrite
 *          or hardware-target change, MINOR on new features, PATCH on bug fixes only.
 *
 * @note    Not currently referenced anywhere in 1A8-UFC.ino — the on-screen boot splash
 *          HW/FW strings (BOOT_OP_MESSAGES[3]/[4] in the main .ino) are separate, 4-char
 *          display-only values and are not derived from this define. Kept here as the
 *          canonical version string for documentation/Doxygen purposes; wire it into the
 *          boot splash or Serial output directly if a single source of truth is wanted.
 */

#ifndef VERSION_H
#define VERSION_H

/** @brief Human-readable firmware version string (semantic versioning). */
#define VERSION_STRING "2.0.0"

#endif // VERSION_H
