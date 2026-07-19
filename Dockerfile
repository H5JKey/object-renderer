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
&& pacman -Scc --noconfirm

COPY renderer/CMakeLists.txt .
RUN mkdir build && cd build && cmake ..

COPY renderer/lib/ lib/
COPY renderer/tests/ tests/
COPY renderer/include/ include/
COPY renderer/src/ src/
RUN cd build && make -j$(nproc)

RUN mkdir build && cd build && cmake ..

CMD ["./build/renderer"]
