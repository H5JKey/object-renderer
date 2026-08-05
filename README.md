# PriZm

[![C++](https://img.shields.io/badge/C++-23-blue?logo=cplusplus&style=flat-square)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C?logo=cmake)](https://cmake.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.6-green?logo=opengl&style=flat-square)](https://www.opengl.org/)
[![Python](https://img.shields.io/badge/Python-3.9+-3776AB?logo=python&style=flat-square)](https://www.python.org/)
[![Docker](https://img.shields.io/badge/Docker-ready-blue?logo=docker&style=flat-square)](https://www.docker.com/)

> Distributed physically based rendering platform

PriZm is an open-source rendering platform that combines a physically based path tracing engine written in C++ with a scalable backend for distributed rendering written in Python.

## Repository Structure

```text
app/          Backend service
renderer/     Rendering engine (standalone CLI and worker)
```

- **Renderer** - a path tracing engine that can run as s standalone CLI application or as a distributed worker.
- **Backend** - manages rendering jobs, storage, client communications  and other server-side functionality.

### Renderer

See the [Renderer documentation](renderer/README.md).

### Backend

See the [Backend documentation](app/README.md).