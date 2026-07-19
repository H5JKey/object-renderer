FROM archlinux:latest

WORKDIR /renderer

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
RUN mkdir build && cd build && cmake ..
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]