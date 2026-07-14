#version 430
layout(local_size_x = 16, local_size_y = 16) in;
layout(rgba16f, binding = 0) uniform readonly image2D inputTexture;
layout(rgba8, binding = 1) uniform writeonly image2D outputTexture;

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size =  imageSize(inputTexture);
    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }

    vec3 color = imageLoad(inputTexture, pixel).rgb;
    color = color / (vec3(1.0) + color);
    
    color = pow(color, vec3(1.0/2.2));

    imageStore(outputTexture, pixel, vec4(color, 1.0));
}