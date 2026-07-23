FROM alpine:latest

WORKDIR /renderer

ENV DEBUGINFOD_URLS="https://debuginfod.archlinux.org"
RUN apk update && \
    apk add --no-cache \
    gcc \
    g++ \
    make \
    cmake \
    git \
    mesa \
    mesa-utils \
    mesa-dev \
    mesa-egl \
    glu \
    glu-dev \
    py3-jinja2 \
    ispc \
    openimagedenoise-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    valgrind \
    && rm -rf /var/cache/apk/*

COPY renderer/ .
RUN mkdir build && cd build && cmake ..
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]