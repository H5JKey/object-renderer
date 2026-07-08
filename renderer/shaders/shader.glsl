#version 430 core

layout(rgba8, binding = 0) uniform image2D outputImage;

layout(local_size_x = 8, local_size_y = 8) in;

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imageSize = imageSize(outputImage);

    if (pixel.x >= imageSize.x || pixel.y >= imageSize.y) {
        return;
    }

    vec2 uv = vec2(pixel) / vec2(imageSize);

    vec3 color = vec3(uv.x, uv.y * 0.5, 1.0 - uv.x);

    imageStore(outputImage, pixel, vec4(1.0,0.0,0.0, 1.0));
}