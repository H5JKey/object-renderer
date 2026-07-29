# PriZm

[![C++](https://img.shields.io/badge/C++-23-blue?logo=cplusplus&style=flat-square)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C?logo=cmake)](https://cmake.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.6-green?logo=opengl&style=flat-square)](https://www.opengl.org/)
[![Docker](https://img.shields.io/badge/Docker-ready-blue?logo=docker&style=flat-square)](https://www.docker.com/)

**Headless OpenGL compute shader Path Tracer using EGL**

An offline renderer that runs entirely without a display server,
using EGL for GPU-accelerated rendering in headless environments

## Key features:
- GLSL compute shaders for path tracing
- EGL for headless rendering
- .glb/.gltf scene loading
- Open Image Denoiser
- Debug images output: raw, albedo, normals
- Logging system
- Unit tests
- Docker support

## Build

### 1. Build with Docker

```bash
docker build -t renderer .
docker run --rm renderer --help
```

### 2. Native build
```bash
cd renderer
mkdir build && cd build
cmake ..
cmake --build .
./renderer --help
```
