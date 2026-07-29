# Zyngine

Lightweight, platform-agnostic embedded graphics / UI engine.

Applications talk to Zyngine APIs only. Platform SDKs (Raylib, ESP-IDF, future Arduino)
stay behind the HAL in `src/platform/<name>/`.

```
Application → Widgets / Clay layout → Graphics Core → HAL → Platform → Vendor SDK
```

## Platforms (Stage 1)

| Platform | Status | Build |
|----------|--------|-------|
| Desktop (Raylib) | Working | CMake |
| ESP-IDF | Working layout | `idf.py` |
| Arduino | Stub only | see `src/platform/arduino/README.md` |

## Language

C++20 is required because [Clay](https://github.com/nicbarker/clay) needs it. Engine/HAL
code otherwise sticks to C++17-friendly patterns.

```bash
cmake -B build -DZYN_PLATFORM=desktop -DZYN_BUILD_EXAMPLES=ON
cmake --build build -j
./build/desktop_smoke
```

## ESP-IDF build

Export your ESP-IDF environment, then:

```bash
cd examples/espidf_smoke
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

LovyanGFX is pulled via the example’s `idf_component.yml`. Board pins live in
`boards/esp32_s3_zune/board.hpp`.

## UI: Clay + Retro TUI theme

- Layout: vendored [Clay](https://github.com/nicbarker/clay) (`third_party/clay/clay.h`)
- Rendering: **one** shared Clay → Graphics Core mapper (no Clay-Raylib path)
- Theme: amber CRT by default; green phosphor via `Theme::setGreen()`
- Widgets: `drawLabel`, `drawButton`, `drawPanel` (behavior + Clay declarations only)

Clay arena defaults are intentionally small for embedded (see `ClayHostConfig`). Raise
`maxElements` / `arenaBytes` on desktop; on ESP32-S3 prefer PSRAM for larger arenas.

## Layout

```
include/zyngine/   public headers (core, graphics, ui, hal, board)
src/               implementations + platform backends
boards/            pin / display config (no parser yet)
examples/          desktop_smoke, espidf_smoke, legacy (parked)
components/zyngine ESP-IDF component wrapper
third_party/clay   Clay single header
```

## Legacy apps

Old PlatformIO Tetris / demos are under `examples/legacy/` and are **not** built by default.
They still need a port to the new Engine / HAL APIs.

## License / history

Previously built with PlatformIO + Arduino. PlatformIO has been removed; use CMake / idf.py.
