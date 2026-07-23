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
    libopenimagedenoise-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

COPY renderer/ .

RUN mkdir build && cd build && cmake .. 
RUN cd build && make -j$(nproc)

CMD ["./build/renderer"]