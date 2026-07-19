#include "scene.hpp"

Scene::Scene()
    : origin(0.1f, 5.0f, 5.0f),
      lookAt(5, 5, 5),
      sunDirection(-10.0f, 3.0f, -10.0f),
      sunColor(0.0f, 0.0f, 0.0f),
      backgroundColor(0.7f, 0.7f, 0.7f) {
    materials.emplace_back(vec3(0.725, 0.71, 0.68), vec3(0, 0, 0), 0.0, 0.7, 0.0, 1.5);
    materials.emplace_back(vec3(0.12, 0.45, 0.15), vec3(0, 0, 0), 0.0, 0.7, 0.0, 1.5);
    materials.emplace_back(vec3(0.65, 0.05, 0.05), vec3(0, 0, 0), 0.0, 0.7, 0.0, 1.5);
    materials.emplace_back(vec3(0.8, 0.8, 0.8), vec3(10.0, 10.0, 10.0), 0.0, 0.1, 0.0, 0.0);
    materials.emplace_back(vec3(0.65, 0.65, 0.65), vec3(0, 0, 0), 0.0, 0.85, 0.0, 1.5);
    materials.emplace_back(vec3(0.7, 0.7, 0.7), vec3(0, 0, 0), 1.0, 0.85, 0.0, 1.5);

    vertices.emplace_back(0, 0, 0);
    vertices.emplace_back(0, 0, 10);
    vertices.emplace_back(0, 10, 0);
    vertices.emplace_back(0, 10, 10);
    vertices.emplace_back(10, 0, 0);
    vertices.emplace_back(10, 0, 10);
    vertices.emplace_back(10, 10, 0);
    vertices.emplace_back(10, 10, 10);

    vertexIndices.push_back(2);
    vertexIndices.push_back(1);
    vertexIndices.push_back(0);
    materialIndices.push_back(0);

    vertexIndices.push_back(1);
    vertexIndices.push_back(2);
    vertexIndices.push_back(3);
    materialIndices.push_back(0);

    vertexIndices.push_back(5);
    vertexIndices.push_back(1);
    vertexIndices.push_back(3);
    materialIndices.push_back(2);

    vertexIndices.push_back(5);
    vertexIndices.push_back(3);
    vertexIndices.push_back(7);
    materialIndices.push_back(2);

    vertexIndices.push_back(5);
    vertexIndices.push_back(7);
    vertexIndices.push_back(4);
    materialIndices.push_back(0);

    vertexIndices.push_back(7);
    vertexIndices.push_back(6);
    vertexIndices.push_back(4);
    materialIndices.push_back(0);

    vertexIndices.push_back(2);
    vertexIndices.push_back(0);
    vertexIndices.push_back(6);
    materialIndices.push_back(1);

    vertexIndices.push_back(0);
    vertexIndices.push_back(4);
    vertexIndices.push_back(6);
    materialIndices.push_back(1);

    vertexIndices.push_back(3);
    vertexIndices.push_back(2);
    vertexIndices.push_back(6);
    materialIndices.push_back(0);

    vertexIndices.push_back(3);
    vertexIndices.push_back(6);
    vertexIndices.push_back(7);
    materialIndices.push_back(0);

    vertexIndices.push_back(0);
    vertexIndices.push_back(1);
    vertexIndices.push_back(4);
    materialIndices.push_back(0);

    vertexIndices.push_back(5);
    vertexIndices.push_back(4);
    vertexIndices.push_back(1);
    materialIndices.push_back(0);

    vertices.emplace_back(4, 5.5, 0.01);
    vertices.emplace_back(6, 5.5, 0.01);
    vertices.emplace_back(6, 7.5, 0.01);
    vertices.emplace_back(4, 7.5, 0.01);

    vertexIndices.push_back(8);
    vertexIndices.push_back(9);
    vertexIndices.push_back(10);
    materialIndices.push_back(3);

    vertexIndices.push_back(11);
    vertexIndices.push_back(8);
    vertexIndices.push_back(10);
    materialIndices.push_back(3);

    vertices.emplace_back(8 + 1.414f, 0, 8);
    vertices.emplace_back(8, 0, 8 - 1.414f);
    vertices.emplace_back(8 - 1.414f, 0, 8);
    vertices.emplace_back(8, 0, 8 + 1.414f);

    vertices.emplace_back(8 + 1.414f, 4, 8);
    vertices.emplace_back(8, 4, 8 - 1.414f);
    vertices.emplace_back(8 - 1.414f, 4, 8);
    vertices.emplace_back(8, 4, 8 + 1.414f);

    vertexIndices.push_back(15);
    vertexIndices.push_back(14);
    vertexIndices.push_back(12);
    materialIndices.push_back(4);

    vertexIndices.push_back(14);
    vertexIndices.push_back(13);
    vertexIndices.push_back(12);
    materialIndices.push_back(4);

    vertexIndices.push_back(16);
    vertexIndices.push_back(18);
    vertexIndices.push_back(19);
    materialIndices.push_back(4);

    vertexIndices.push_back(16);
    vertexIndices.push_back(17);
    vertexIndices.push_back(18);
    materialIndices.push_back(4);

    vertexIndices.push_back(19);
    vertexIndices.push_back(14);
    vertexIndices.push_back(18);
    materialIndices.push_back(4);

    vertexIndices.push_back(19);
    vertexIndices.push_back(15);
    vertexIndices.push_back(14);
    materialIndices.push_back(4);

    vertexIndices.push_back(17);
    vertexIndices.push_back(14);
    vertexIndices.push_back(13);
    materialIndices.push_back(4);

    vertexIndices.push_back(17);
    vertexIndices.push_back(18);
    vertexIndices.push_back(14);
    materialIndices.push_back(4);

    vertexIndices.push_back(16);
    vertexIndices.push_back(17);
    vertexIndices.push_back(13);
    materialIndices.push_back(4);

    vertexIndices.push_back(16);
    vertexIndices.push_back(13);
    vertexIndices.push_back(12);
    materialIndices.push_back(4);

    vertexIndices.push_back(19);
    vertexIndices.push_back(16);
    vertexIndices.push_back(15);
    materialIndices.push_back(4);

    vertexIndices.push_back(16);
    vertexIndices.push_back(12);
    vertexIndices.push_back(15);
    materialIndices.push_back(4);

    vertices.emplace_back(7 + 1.5 * 0.866f, 0, 3 + 1.5 * 0.5f);
    vertices.emplace_back(7 - 1.5 * 0.5f, 0, 3 + 1.5 * 0.866f);
    vertices.emplace_back(7 - 1.5 * 0.866f, 0, 3 - 1.5 * 0.5f);
    vertices.emplace_back(7 + 1.5 * 0.5f, 0, 3 - 1.5 * 0.866f);

    vertices.emplace_back(7 + 1.5 * 0.866f, 3, 3 + 1.5 * 0.5f);
    vertices.emplace_back(7 - 1.5 * 0.5f, 3, 3 + 1.5 * 0.866f);
    vertices.emplace_back(7 - 1.5 * 0.866f, 3, 3 - 1.5 * 0.5f);
    vertices.emplace_back(7 + 1.5 * 0.5f, 3, 3 - 1.5 * 0.866f);

    vertexIndices.push_back(23);
    vertexIndices.push_back(22);
    vertexIndices.push_back(20);
    materialIndices.push_back(5);

    vertexIndices.push_back(22);
    vertexIndices.push_back(21);
    vertexIndices.push_back(20);
    materialIndices.push_back(5);

    vertexIndices.push_back(27);
    vertexIndices.push_back(26);
    vertexIndices.push_back(24);
    materialIndices.push_back(5);

    vertexIndices.push_back(26);
    vertexIndices.push_back(25);
    vertexIndices.push_back(24);
    materialIndices.push_back(5);

    vertexIndices.push_back(27);
    vertexIndices.push_back(22);
    vertexIndices.push_back(26);
    materialIndices.push_back(5);

    vertexIndices.push_back(27);
    vertexIndices.push_back(23);
    vertexIndices.push_back(22);
    materialIndices.push_back(5);

    vertexIndices.push_back(25);
    vertexIndices.push_back(22);
    vertexIndices.push_back(21);
    materialIndices.push_back(5);

    vertexIndices.push_back(25);
    vertexIndices.push_back(26);
    vertexIndices.push_back(22);
    materialIndices.push_back(5);

    vertexIndices.push_back(24);
    vertexIndices.push_back(25);
    vertexIndices.push_back(21);
    materialIndices.push_back(5);

    vertexIndices.push_back(24);
    vertexIndices.push_back(21);
    vertexIndices.push_back(20);
    materialIndices.push_back(5);

    vertexIndices.push_back(27);
    vertexIndices.push_back(24);
    vertexIndices.push_back(23);
    materialIndices.push_back(5);

    vertexIndices.push_back(24);
    vertexIndices.push_back(20);
    vertexIndices.push_back(23);
    materialIndices.push_back(5);
}