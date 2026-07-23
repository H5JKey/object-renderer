FROM alpine:latest

WORKDIR /renderer

ENV DEBUGINFOD_URLS="https://debuginfod.archlinux.org"
RUN apk update && \
    apk add --no-cache \
    build-base \
    cmake \
    git \
    mesa \
    mesa-dev \
    mesa-egl \
    mesa-gles \
    mesa-dri-gallium \
    mesa-gl \
    py3-jinja2 \
    ispc \
    libxcb \
    libx11-dev \
    openimagedenoise-dev \
    valgrind \
    && rm -rf /var/cache/apk/*

RUN apk add --no-cache \
    libgl \
    libgl-dev \
    libgles \
    libgles-dev \
    libegl \
    libegl-dev \
    mesa-dev \
    mesa-gl \
    mesa-egl \
    mesa-gles \
    libglvnd \
    libglvnd-dev \
    glu \
    glu-dev

COPY renderer/ .
RUN mkdir build && cd build && cmake ..
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]