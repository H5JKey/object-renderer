FROM ubuntu:latest

WORKDIR /renderer

RUN apt-get update && apt-get install --no-install-recommends -y software-properties-common

RUN add-apt-repository universe

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3-jinja2 \
    ispc \
    valgrind \
    libgl1-mesa-dev \
    libglvnd-dev \
    libx11-dev \
    libxcb1-dev \
    libdrm-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --recursive https://github.com/RenderKit/oidn.git && \
    cd oidn && \
    mkdir build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DCMAKE_INSTALL_LIBDIR=lib/x86_64-linux-gnu \
        -DCMAKE_INSTALL_INCLUDEDIR=include && \
    make -j$(nproc) && \
    make install && \
    cd / && rm -rf oidn

COPY renderer/ .

RUN mkdir build && cd build && cmake .. 
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]