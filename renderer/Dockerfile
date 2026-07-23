FROM alpine:3.24

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

COPY renderer/ .
RUN mkdir build && cd build && cmake ..
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]