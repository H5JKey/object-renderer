FROM alpine:latest

WORKDIR /renderer

ENV EGL_PLATFORM=surfaceless

RUN apk update && apk add \
    cmake \
    ca-certificates \
    mesa-dev \
    mesa-gl \
    py3-jinja2 \
    mesa-dri-gallium \
    ispc \
    openimagedenoise \
    && rm -rf /var/cache/apk/* \

COPY renderer .

RUN mkdir build && cd build && cmake .. && cmake --build .

CMD ["./build/renderer"]