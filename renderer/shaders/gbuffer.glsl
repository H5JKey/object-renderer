#version 430 core

layout(rgba32f, binding = 0) uniform writeonly image2D normalMap;
layout(rgba32f, binding = 1) uniform writeonly image2D albedoMap;

struct Material {
    vec4 albedo;
    vec4 emission;
    float metalness;
    float roughness;
    float transmission;
    float ior;
};

layout(std430, binding = 2) buffer vertexBuffer {
    vec4 vertices[];
};

layout(std430, binding = 3) buffer vertexIndexBuffer {
    int verticesIndices[];
};

layout(std430, binding = 4) buffer materialBuffer {
    Material materials[];
};

layout(std430, binding = 5) buffer materialIndexBuffer {
    int materialsIndices[];
};

uniform int uCount;
uniform vec3 uOrigin;
uniform float uFovDegrees;
uniform vec3 uLookAt;

layout(local_size_x = 16, local_size_y = 16) in;


struct HitInfo {
    float distance;
    vec3 position;
    int material_id;
    vec3 normal;
};

HitInfo triangle_intersection(vec3 ro, vec3 rd, int triangle_idx) {
    HitInfo info;
    
    vec3 v0 = vertices[verticesIndices[3*triangle_idx]].xyz;
    vec3 v1 = vertices[verticesIndices[3*triangle_idx+1]].xyz;
    vec3 v2 = vertices[verticesIndices[3*triangle_idx+2]].xyz;

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec3 pvec = cross(rd, e2);
    float det = dot(e1, pvec);

    if (abs(det) < 1e-8) {
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
    info.normal = normalize(cross(v1 - v0, v2 - v0));
    info.material_id = materialsIndices[triangle_idx];
    return info;
}


uint pcg(uint seed) {
    uint state = seed * uint(747796405) + uint(2891336453u);
	uint word = ((state >> ((state >> uint(28)) + uint(4))) ^ state) * uint(277803737);
	return (word >> uint(22)) ^ word;
}

float random(uint seed) {
    return pcg(seed) / float(uint(0xffffffff));
}

vec2 random2(uint seed) {
    return vec2(random(seed), random(seed+1));
}

const float MAX_DIST=100000.0;
HitInfo castRay(vec3 origin, vec3 direction) {
    HitInfo closestHitInfo;
    closestHitInfo.distance = MAX_DIST;
    closestHitInfo.position = vec3(0);
    closestHitInfo.normal = vec3(0);
    closestHitInfo.material_id = -1;
    for (int i=0; i<uCount; i++) {
        HitInfo info = triangle_intersection(origin, direction, i);
        if (info.distance > 0.0 && info.distance < closestHitInfo.distance) {
            closestHitInfo = info;
        }
    }
    return closestHitInfo;
}

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size =  imageSize(normalMap);

    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }

    float fov = tan(uFovDegrees * 0.5 * 3.141592 / 180.0);

    vec3 forward = normalize(uLookAt - uOrigin);
    vec3 right = cross(vec3(0.0, 1.0, 0.0), forward);
    vec3 up = cross(forward, right);

    vec2 uv = (2.0*vec2(pixel) - vec2(size)) / vec2(size).y;

    uv.y = -uv.y;

    vec3 direction = normalize(forward + right * fov * uv.x + up * fov * uv.y);

    HitInfo hit = castRay(uOrigin, direction);

    imageStore(normalMap, pixel, vec4(hit.normal, 1.0));
    if (hit.distance == MAX_DIST) {
        imageStore(albedoMap, pixel, vec4(0,0,0, 1.0));
    }
    else {
        imageStore(albedoMap, pixel, vec4(materials[hit.material_id].albedo.rgb, 1.0));
    }
    
}
