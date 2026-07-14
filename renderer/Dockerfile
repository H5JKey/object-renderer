FROM gcc:latest

WORKDIR /renderer

ENV EGL_PLATFORM=surfaceless

ENV LIBGL_ALWAYS_SOFTWARE=1

RUN apt-get update && apt-get install -y \
    cmake \
    wget \
    curl \
    ca-certificates \
    libegl1-mesa-dev \
    libgles2-mesa-dev \
    libgl1-mesa-dev \
    libglvnd-dev \
    libglvnd0 \
    mesa-utils \
    python3-jinja2 \
    && rm -rf /var/lib/apt/lists/*

COPY renderer .

RUN mkdir build && cd build && cmake .. && cmake --build .

CMD ["./build/renderer"]