#version 430 core

layout(rgba8, binding = 0) uniform image2D outputImage;

layout(std430, binding = 1) buffer vertexBuffer {
    vec3 vertices[];
};

layout(std430, binding = 2) buffer indexBuffer {
    int indices[];
};

uniform int uCount;
uniform vec3 uOrigin;
uniform float uFovDegrees;
uniform vec3 uLookAt;
uniform vec3 uSunDirection;
uniform vec3 uBackgroundColor;
uniform vec3 uSunColor;

layout(local_size_x = 16, local_size_y = 16) in;


struct HitInfo {
    float distance;
    vec3 position;
    int material_id;
    vec3 normal;
};

HitInfo triangle_intersection(vec3 ro, vec3 rd, int triangle_idx) {
    HitInfo info;

    vec3 v0 = vertices[indices[3*triangle_idx]];
    vec3 v1 = vertices[indices[3*triangle_idx+1]];
    vec3 v2 = vertices[indices[3*triangle_idx+2]];

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec3 pvec = cross(rd, e2);
    float det = dot(e1, pvec);

    if (det < 1e-8 && det > -1e-8) {
        info.distance = -1.0;
        info.position = vec3(0.0);
        info.normal = vec3(0.0);
        info.material_id = 0;
        return info;
    }

    float inv_det = 1.0 / det;
    vec3 tvec = ro - v0;

    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0 || u > 1.0) {
        info.distance = -1.0;
        info.position = vec3(0.0);
        info.normal = vec3(0.0);
        info.material_id = 0;
        return info;
    }

    vec3 qvec = cross(tvec, e1);

    float v = dot(rd, qvec) * inv_det;
    if (v < 0.0 || u + v > 1.0) {
        info.distance = -1.0;
        info.position = vec3(0.0);
        info.normal = vec3(0.0);
        info.material_id = 0;
        return info;
    }

    float t = dot(e2, qvec) * inv_det;
    if (t < 1e-8) {
        info.distance = -1.0;
        info.position = vec3(0.0);
        info.normal = vec3(0.0);
        info.material_id = 0;
        return info;
    }
    info.distance = t;
    info.position = ro + rd * t;
    info.normal = cross(v1 - v0, v2 - v0);
    info.material_id = 0;
    return info;
}

vec3 getBackgroundColor(vec3 direction) {
    return uBackgroundColor + uSunColor * pow(max(0.0, dot(direction, normalize(uSunDirection))), 256.0);
}

vec3 traceRay(vec3 origin, vec3 direction) {
    const int MAX_DIST=100000;
    HitInfo closestHitInfo;
    closestHitInfo.distance = MAX_DIST;
    for (int i=0; i<uCount; i++) {
        HitInfo info = triangle_intersection(origin, direction, i);
        if (info.distance > 0.0 && info.distance < closestHitInfo.distance) {
            closestHitInfo = info;
        }
    }
    if (closestHitInfo.distance < MAX_DIST) {
        return vec3(1.0,0.0,0.0);
    }
    else {
        return getBackgroundColor(direction);
    }
}


uint pcg(uint seed) {
    uint state = seed * uint(747796405) + uint(2891336453);
	uint word = ((state >> ((state >> uint(28)) + uint(4))) ^ state) * uint(277803737);
	return (word >> uint(22)) ^ word;
}

float random(uint seed) {
    return pcg(seed) / float(uint(0xffffffff));
}

vec2 random2(uint seed) {
    return vec2(random(seed), random(seed+1));
}



void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size =  imageSize(outputImage);

    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }
    float fov = tan(uFovDegrees * 0.5 * 3.141592 / 180.0);

    vec3 forward = normalize(uLookAt - uOrigin);
    vec3 right = cross(vec3(0.0, 1.0, 0.0), forward);
    vec3 up = cross(forward, right);

    vec3 color = vec3(0.0);
    const int samples = 32;
    uint seed = uint(pixel.x) * 10000u + uint(pixel.y) * 100u;
    for (int i = 0; i < samples; i++) {
        seed = seed * 10 + uint(i);
        vec2 jitter = random2(seed) - vec2(0.5, 0.5);
        vec2 uv = (2.0*vec2(pixel) + jitter - vec2(size)) / vec2(size).y;
        uv.y = -uv.y;

        vec3 direction = normalize(forward + right * fov * uv.x + up * fov * uv.y);

        color += traceRay(uOrigin, direction);
    }
    color /= samples;
    imageStore(outputImage, pixel, vec4(color, 1.0));
}