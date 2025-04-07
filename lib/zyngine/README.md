# Zyngine

Zyngine is a lightweight 3D game and physics engine designed for embedded systems like the ESP32. It provides essential tools for rendering, physics, and game development. Provides cross platform compilation to test capabilities on native x86 computers for rapid development.

## Features

- Modular design for flexibility.
- Optimized for performance on constrained hardware.
- Support for basic 3D rendering and physics.

## Directory Structure

```
docs/               - Documentation files.
examples/           - Example projects using Zyngine.
include/            - Header files for the engine.
src/                - Source files for the engine.
config_user.h       - User configuration file.
zyngine.h           - Main engine header file.
zynmath.h           - Math utilities and structures.
zynrenderer.h       - Rendering utilities.
zyntexture.h        - Texture handling utilities.
zynutils.h          - General utilities.
```

## Implemented Features

- [x] Math structs (e.g., vectors, matrices)
- [x] Renderer
- [x] Triangle rasterizer
- [x] Textured triangle rasterizer
- [x] Perspective projection
- [x] Affine texture mapping
- [x] Asset optimizer

## TODO

- [ ] Perspective correct texture mapping
- [ ] Camera controller (controls and matrices)
- [ ] Physics (colliders, rigid bodies, etc.)
- [ ] Map loader
- [ ] Layout tool

## Getting Started

To use Zyngine, include the necessary headers in your project and link against the library. Refer to the `examples/` directory for sample usage.

## License

[Specify your license here, e.g., MIT, GPL, etc.]

## Contributing

Contributions are welcome! Please submit a pull request or open an issue for discussion.
