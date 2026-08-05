# PriZm

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