#version 430 core

layout(rgba8, binding = 0) uniform image2D outputImage;

struct Material {
    vec4 albedo;
    float metalness;
    float roughness;
};

layout(std430, binding = 1) buffer vertexBuffer {
    vec4 vertices[];
};

layout(std430, binding = 2) buffer vertexIndexBuffer {
    int verticesIndices[];
};

layout(std430, binding = 3) buffer materialBuffer {
    Material materials[];
};

layout(std430, binding = 4) buffer materialIndexBuffer {
    int materialsIndices[];
};

uniform int uCount;
uniform vec3 uOrigin;
uniform float uFovDegrees;
uniform vec3 uLookAt;
uniform vec3 uSunDirection;
uniform vec3 uBackgroundColor;
uniform vec3 uSunColor;
uniform uint uSeed;
uniform uint uFrameIndex;

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
    info.normal = normalize(cross(v1 - v0, v2 - v0));
    info.material_id = materialsIndices[triangle_idx];
    return info;
}

vec3 getBackgroundColor(vec3 direction) {
    return uBackgroundColor + uSunColor * pow(max(0.0, dot(direction, normalize(uSunDirection))), 256.0);
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

vec3 randomOnSphere(uint seed){
  vec3 rand = vec3(random(seed+67),random(seed+42),random(seed+52));
  
  float theta = rand.x * 2.0 * 3.14159265;
  float v = rand.y;
  float phi = acos (2.0 * v-1.0);
  float r = pow(rand.z,1.0 / 3.0);
  float x=r*sin(phi)*cos (theta);
  float y=r* sin(phi)*sin(theta);
  float z=r*cos(phi);
  return vec3(x, y, z);
}

const float MAX_DIST=100000.0;
HitInfo castRay(vec3 origin, vec3 direction) {
    HitInfo closestHitInfo;
    closestHitInfo.distance = MAX_DIST;
    for (int i=0; i<uCount; i++) {
        HitInfo info = triangle_intersection(origin, direction, i);
        if (info.distance > 0.0 && info.distance < closestHitInfo.distance) {
            closestHitInfo = info;
        }
    }
    return closestHitInfo;
}

vec3 schlickFresnel(vec3 V, vec3 H, vec3 R0) {
    float cosTheta = max(dot(H, V), 0.0);
    return R0 + (vec3(1.0) - R0) * pow((1 - cosTheta), 5.0);
}

vec3 GGX(float u1, float u2, vec3 N, float roughness) {
    float theta = atan(roughness * roughness * sqrt(u1) / sqrt(1 - u1));
    float phi = 2 * 3.141592 * u2;

    float sinTheta = sin(theta);
    vec3 local = vec3(sinTheta * cos(phi), cos(theta), sinTheta * sin(phi));
    vec3 T;
    if (abs(N.z) < 0.999) {
        T = normalize(cross(vec3(0,0,1), N));
    }
    else {
        T = normalize(cross(vec3(1,0,0), N));
    }
    vec3 B = normalize(cross(N, T));

    return normalize(local.x * T + local.y * N + local.z * B);
}

float distribution(vec3 H, vec3 N, float roughness) {
    float a = roughness * roughness;
    float a2 = a*a;
    return a2 / (3.141592 * pow((pow(dot(N,H), 2.0) * (a2 - 1.0) + 1.0), 2.0));
}

float schlickGGX(vec3 N, vec3 V, float roughness) {
    float k = pow((roughness + 1.0), 2.0) / 8.0;
    return dot(N,V) / (dot(N,V) * (1.0 - k) + k);
}

float smith(vec3 N, vec3 V, vec3 L, float roughness) {
    return schlickGGX(N, V, roughness) * schlickGGX(N, L, roughness);
}

vec3 traceRay(vec3 origin, vec3 direction, uint seed) {
    vec3 color = vec3(0,0,0);
    vec3 throughput = vec3(1.0);
    const int bounces = 16;
    for (int i=0; i<bounces; i++) {
        seed = pcg(seed);
        HitInfo hit = castRay(origin, direction);
        if (hit.distance == MAX_DIST) {
            return throughput * getBackgroundColor(direction);
        }
        Material material = materials[hit.material_id];
        if (material.metalness > 0.0) {
            if (material.roughness > 0.01) {
                vec3 H,V,N,L;
                do {
                    float u1 = random(seed);
                    float u2 = random(pcg(seed+1u));
                    H = GGX(u1, u2, hit.normal, material.roughness);
                    V = normalize(-direction);
                    N = hit.normal;
                    L = reflect(-V, H);
                    seed = pcg(seed);
                } while (dot(L, N) <= 0.0);
                vec3 F = schlickFresnel(V, H, material.albedo.xyz);
                float G = smith(N, V, L, material.roughness);
                float D = distribution(H, N, material.roughness);
                vec3 fr = (D * F * G) / (4 * dot(N,V) * dot(N, L));
                float pdf = D * dot(N,H) / (4 * dot(V,H));
                throughput *= fr * dot (N, L) / pdf;
                origin = hit.position + hit.normal * 0.001; 
                direction = L;
            } else {
                throughput *= schlickFresnel(-direction, hit.normal, material.albedo.xyz);
                origin = hit.position + hit.normal * 0.001;
                direction = reflect(direction, hit.normal);
            }
        }
        else {
            
        }
    }
    return vec3(0.0);
}


void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size =  imageSize(outputImage);

    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }
    vec3 oldColor = imageLoad(outputImage, pixel).rgb;
    float fov = tan(uFovDegrees * 0.5 * 3.141592 / 180.0);

    vec3 forward = normalize(uLookAt - uOrigin);
    vec3 right = cross(vec3(0.0, 1.0, 0.0), forward);
    vec3 up = cross(forward, right);

    vec3 color = vec3(0.0);
    const int samples = 32;
    for (int i = 0; i < samples; i++) {
        uint seed = uSeed + uint(i) + 6732 * uint(pixel.x) + 8157 * uint(pixel.y);
        vec2 jitter = random2(seed) - vec2(0.5, 0.5);
        vec2 uv = (2.0*vec2(pixel) + jitter - vec2(size)) / vec2(size).y;
        uv.y = -uv.y;

        vec3 direction = normalize(forward + right * fov * uv.x + up * fov * uv.y);

        color += traceRay(uOrigin, direction, seed);
    }
    color /= samples;
    color = mix(oldColor, color, 1.0 / (uFrameIndex + 1));
    imageStore(outputImage, pixel, vec4(color, 1.0));
}
