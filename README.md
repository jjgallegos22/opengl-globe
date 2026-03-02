# OpenGL Globe

A 3D rotating Earth rendered in C++ using OpenGL. The globe is procedurally generated as a UV sphere and texture-mapped with an equirectangular Earth image.

## Dependencies

- [GLEW](http://glew.sourceforge.net/) — OpenGL extension wrangler
- [GLFW](https://www.glfw.org/) — window and input handling
- [GLM](https://github.com/g-truc/glm) — math library for OpenGL

The easiest way to install these on Windows is via [vcpkg](https://github.com/microsoft/vcpkg):

```bash
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg integrate install
C:\vcpkg\vcpkg install glew:x64-windows glfw3:x64-windows glm:x64-windows
```

## Building

Open the solution in **Visual Studio** (2019 or 2022 recommended) and build in **Debug** or **Release** mode targeting **x64**.

The working directory must be set to the project root (the folder containing `resources/`) so the texture can be found at runtime. In Visual Studio this is configured under:

> Project Properties → Debugging → Working Directory

## Controls

| Key | Mode |
|-----|------|
| `1` | Point cloud |
| `2` | Wireframe |
| Any other key | Filled (textured) |

## Resources

- `resources/earth.bmp` — daytime Earth texture (equirectangular projection)
- `resources/night.bmp` — nighttime Earth texture (unused)
- `resources/sun.bmp` — sun texture (unused)
