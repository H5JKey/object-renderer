FROM archlinux:latest

ARG BUILD_MODE
ARG BUILD_TESTS=OFF

WORKDIR /renderer

ENV DEBUGINFOD_URLS="https://debuginfod.archlinux.org"
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
    gcc \
    make \
    cmake \
    git \
    mesa \
    mesa-utils \
    python-jinja \
    ispc \
    openimagedenoise \
    libglvnd \
    libx11 \
    libxrandr \
    libxinerama \
    libxcursor \
    libxi \
    valgrind \
&& pacman -Scc --noconfirm

COPY renderer/ .
RUN mkdir build && cd build && cmake .. -DBUILD_MODE=${BUILD_MODE} -DBUILD_TESTS=${BUILD_TESTS}
RUN cd build && make -j$(nproc)

ENV PATH="/renderer/build:${PATH}"