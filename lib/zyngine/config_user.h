#ifndef ZYNGINE_CONFIG_USER_H
#define ZYNGINE_CONFIG_USER_H

/**
 * @file config_user.h
 * @brief Configuration file for defining the target system.
 *
 * This file contains preprocessor directives to specify the target system
 * for the ZYngine project. Modify these definitions to match the desired
 * target platform.
 *
 * @section Usage
 * - `#define ZYNGINE_ESP32S3`: Define this macro to target the ESP32-S3 platform.
 * - `#define ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER`: Define this macro to target the Windows platform with a custom software renderer using Raylib.
 *
 * @note Ensure only the relevant target system macros are defined to avoid conflicts.
*/

// Ensure only One Target System is defined

// #define ZYNGINE_ESP32S3
#define ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER

#endif // ZYNGINE_CONFIG_USER_H