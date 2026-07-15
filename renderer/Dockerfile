FROM gcc:latest

WORKDIR /renderer

ENV EGL_PLATFORM=surfaceless

RUN apt-get update && apt-get install -y \
    cmake \
    ca-certificates \
    libegl1-mesa-dev \
    libgl1-mesa-dev \
    python3-jinja2 \
    libgl1-mesa-dri \
    && rm -rf /var/lib/apt/lists/*

COPY renderer .

RUN mkdir build && cd build && cmake .. && cmake --build .

CMD ["./build/renderer"]