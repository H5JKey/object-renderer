FROM archlinux:latest

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
    glibc-debug \
&& pacman -Scc --noconfirm

COPY renderer/ .
RUN mkdir build && cd build && cmake ..
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]