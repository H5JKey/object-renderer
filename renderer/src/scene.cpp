#include "scene.hpp"

Scene::Scene() : 
            origin(-45.0f, 40.0f, 45.0f),
            lookAt(0,0,0), 
            sunDirection(-10.0f, 3.0872f, -10.0f),
            sunColor(1.0f, 0.95f, 0.85f), 
            backgroundColor(0.53f, 0.81f, 0.92f) 
{
    materials.emplace_back(vec3(0.8,0.8,0.8),1.0,0.85);
    materials.emplace_back(vec3(0.5,0.5,0.5),1.0,0.15);
    materials.emplace_back(vec3(0.8,0.2,0.2),1.0,0.95);

    vertices.emplace_back(0,0,0);
    vertices.emplace_back(0,0,10);
    vertices.emplace_back(0,10,0);
    vertices.emplace_back(0,10,10);
    vertices.emplace_back(10,0,0);
    vertices.emplace_back(10,0,10);
    vertices.emplace_back(10,10,0);
    vertices.emplace_back(10,10,10);

    vertexIndices.push_back(0);
    vertexIndices.push_back(1);
    vertexIndices.push_back(2);
    materialIndices.push_back(1);

    vertexIndices.push_back(3);
    vertexIndices.push_back(2);
    vertexIndices.push_back(1);
    materialIndices.push_back(1);

    vertexIndices.push_back(3);
    vertexIndices.push_back(1);
    vertexIndices.push_back(5);
    materialIndices.push_back(1);
    
    vertexIndices.push_back(7);
    vertexIndices.push_back(3);
    vertexIndices.push_back(5);
    materialIndices.push_back(1);

    vertexIndices.push_back(4);
    vertexIndices.push_back(7);
    vertexIndices.push_back(5);
    materialIndices.push_back(1);

    vertexIndices.push_back(4);
    vertexIndices.push_back(6);
    vertexIndices.push_back(7);
    materialIndices.push_back(1);

    vertexIndices.push_back(6);
    vertexIndices.push_back(0);
    vertexIndices.push_back(2);
    materialIndices.push_back(1);

    vertexIndices.push_back(6);
    vertexIndices.push_back(4);
    vertexIndices.push_back(0);
    materialIndices.push_back(1);

    vertexIndices.push_back(6);
    vertexIndices.push_back(2);
    vertexIndices.push_back(3);
    materialIndices.push_back(1);

    vertexIndices.push_back(7);
    vertexIndices.push_back(6);
    vertexIndices.push_back(3);
    materialIndices.push_back(1);

    vertexIndices.push_back(4);
    vertexIndices.push_back(1);
    vertexIndices.push_back(0);
    materialIndices.push_back(1);

    vertexIndices.push_back(5);
    vertexIndices.push_back(4);
    vertexIndices.push_back(1);
    materialIndices.push_back(1);

    vertices.emplace_back(-50,0,50);
    vertices.emplace_back(-50,0,-50);
    vertices.emplace_back(50,0,-50);
    vertices.emplace_back(50,0,50);

    vertexIndices.push_back(11);
    vertexIndices.push_back(9);
    vertexIndices.push_back(8);
    materialIndices.push_back(0);

    vertexIndices.push_back(10);
    vertexIndices.push_back(9);
    vertexIndices.push_back(11);
    materialIndices.push_back(0);


    vertices.emplace_back(-10,0,-5);
    vertices.emplace_back(-10,0,-1);
    vertices.emplace_back(-10,6,-5);
    vertices.emplace_back(-10,6,-1);
    vertices.emplace_back(-5,0,-5);
    vertices.emplace_back(-5,0,-1);
    vertices.emplace_back(-5,6,-5);
    vertices.emplace_back(-5,6,-1);

    vertexIndices.push_back(12);
    vertexIndices.push_back(13);
    vertexIndices.push_back(14);
    materialIndices.push_back(2);

    vertexIndices.push_back(15);
    vertexIndices.push_back(14);
    vertexIndices.push_back(13);
    materialIndices.push_back(2);

    vertexIndices.push_back(15);
    vertexIndices.push_back(13);
    vertexIndices.push_back(17);
    materialIndices.push_back(2);
    
    vertexIndices.push_back(19);
    vertexIndices.push_back(15);
    vertexIndices.push_back(17);
    materialIndices.push_back(2);

    vertexIndices.push_back(16);
    vertexIndices.push_back(19);
    vertexIndices.push_back(17);
    materialIndices.push_back(2);

    vertexIndices.push_back(16);
    vertexIndices.push_back(18);
    vertexIndices.push_back(19);
    materialIndices.push_back(2);

    vertexIndices.push_back(18);
    vertexIndices.push_back(12);
    vertexIndices.push_back(14);
    materialIndices.push_back(2);

    vertexIndices.push_back(18);
    vertexIndices.push_back(16);
    vertexIndices.push_back(12);
    materialIndices.push_back(2);

    vertexIndices.push_back(18);
    vertexIndices.push_back(14);
    vertexIndices.push_back(15);
    materialIndices.push_back(2);

    vertexIndices.push_back(19);
    vertexIndices.push_back(18);
    vertexIndices.push_back(15);
    materialIndices.push_back(2);

    vertexIndices.push_back(16);
    vertexIndices.push_back(13);
    vertexIndices.push_back(12);
    materialIndices.push_back(2);

    vertexIndices.push_back(17);
    vertexIndices.push_back(16);
    vertexIndices.push_back(13);
    materialIndices.push_back(2);
}