# PriZm

[![C++](https://img.shields.io/badge/C++-23-blue?logo=cplusplus&style=flat-square)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C?logo=cmake)](https://cmake.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.6-green?logo=opengl&style=flat-square)](https://www.opengl.org/)
[![Docker](https://img.shields.io/badge/Docker-ready-blue?logo=docker&style=flat-square)](https://www.docker.com/)

## Headless OpenGL compute shader Path Tracer using EGL

An renderer that runs entirely without a display server,
using EGL for GPU-accelerated rendering in headless environments

## Demos

<table>
  <tr>
    <td><img src="images/fox.png" alt="Glass fox" width="400"></td>
    <td><img src="images/barry.png" alt="Barry" width="400"></td>
  </tr>
  <tr>
    <td><img src="images/flight-helmet.png" alt="Flight helmet" width="400"></td>
    <td><img src="images/monument.png" alt="Monument" width="400"></td>
  </tr>
</table>

## Key features:
- **GLSL compute shaders for path tracing**
- **Physically based rendering**
- **EGL for headless rendering**
- **glTF 2.0 loading**
- **Open Image Denoiser**
- **Debug images output: raw, albedo, normals**
- **Logging system**
- **Unit tests with googleTest**
- **Docker ready**

## Build Options

### Build Modes

| Mode | CMake Flag | Output | Description |
|------|-----------|--------|-------------|
| **Core** | `-DBUILD_MODE=CORE` | `-` | Build only core rendering library for testing |
| **CLI** | `-DBUILD_MODE=CLI` | `renderer_cli` | Standalone CLI application |
| **Microservice** | `-DBUILD_MODE=MICROSERVICE` | `renderer_web` | Build renderer worker for web service |
| **Both** | `-DBUILD_MODE=BOTH` | `renderer_cli` + `renderer_web` | Build both CLI and Web service |

### Build Options

| Option | CMake Flag | Description |
|--------|-----------|-------------|
| **Tests** | `-DBUILD_TESTS=ON` | Build unit tests |

## 1. CLI version

### Build

#### Docker

```bash
docker build -t priZm --build-arg BUILD_MODE=CLI  .
```

#### CMake

```bash
cd renderer
mkdir build && cd build
cmake .. -DBUILD_MODE=CLI
cmake --build .
```

### Usage

#### CMake
```bash
./renderer_cli <width> <height> <samples> <input_scene> [OPTIONS]
```

```bash
# example
./renderer_cli 1920 1080 50 tests/data/test-scene.glb -o scene.png --plane 15 --verbose
```

#### Docker
```bash
docker run --rm priZm renderer_cli
```
###  Command line arguments

| Argument | Type | Description |
|----------|------|-------------|
| `width` | `int` | Output image width |
| `height` | `int` | Output image height |
| `samples` | `int` | Number of samples per pixel |
| `input_scene` | `string` | Path to .glb/.gltf scene file |

### Command line options

| Option | Type | Description |
|--------|------|-------------|
| `-h, --help` | `-` | Show help message |
| `-o, --output` | `string` | Output image path (default: output.png) |
| `-v, --verbose` | `-` | Enable detailed logging |
| `-d, --debug` | `-` | Save debug images (raw, albedo, normals) |
| `-p, --plane` | `float` | Add ground plane at scene center with specified size |
| `-c, --camera` | `vec3 vec3 float` | Set camera: `position lookAt fov` |


## 2. Microservice version

### Build

#### Docker
```bash
docker build -t priZm --build-arg BUILD_MODE=MICROSERVICE  .
```

#### CMake
```bash
cd renderer
mkdir build && cd build
cmake .. -DBUILD_MODE=MICROSERVICE
cmake --build .
```
### Usage

#### Docker
```bash
docker run --rm priZm renderer_web
```

#### CMake
```bash
./renderer_web
```
## 3. Testing

#### Docker
```bash
docker build -t priZm --build-arg BUILD_MODE=CORE  --build-arg BUILD_TESTS=ON .
docker run --rm priZm test
```
### CMake
```bash
cd renderer
mkdir build && cd build
cmake .. -DBUILD_MODE=CORE
cmake --build .
./test
```
