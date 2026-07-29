# Future Arduino platform backend

Arduino will be a **third** Zyngine platform backend alongside `desktop` and `espidf`.
It is **not** based on PlatformIO.

## Goal

Implement the same HAL interfaces under `src/platform/arduino/`:

- `Display` (e.g. LovyanGFX Arduino API, or board-specific GFX)
- `Gpio`, `Time`, `Log`, `Mutex`, `Filesystem` (SD / SPIFFS)
- `Input` stubs or touch/encoder

Core, graphics, Clay, and widgets must remain free of `Arduino.h`.

## Suggested wiring

1. Add Arduino sources mirroring `src/platform/desktop/` and `src/platform/espidf/`.
2. Extend `cmake/ZynginePlatform.cmake` to accept `ZYN_PLATFORM=arduino`.
3. Provide an Arduino Library / PlatformIO *optional* consumer later if desired — but the
   framework itself builds with CMake (or Arduino-CLI) against these HAL TUs.

## Status

Not implemented in Stage 1.
