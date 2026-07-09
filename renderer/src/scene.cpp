#include "scene.hpp"

Scene::Scene() : 
            origin(60.0f, 40.0f, 60.0f),
            lookAt(0,0,0), 
            sunDirection(-0.5395f, 0.0872f, -0.8632f),
            sunColor(1.0f, 0.95f, 0.85f), 
            backgroundColor(0.53f, 0.81f, 0.92f) 
{
    vertices.push_back({0,0,0});
    vertices.push_back({0,0,10});
    vertices.push_back({0,10,0});
    vertices.push_back({0,10,10});
    vertices.push_back({10,0,0});
    vertices.push_back({10,0,10});
    vertices.push_back({10,10,0});
    vertices.push_back({10,10,10});

    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(0);

    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(1);

    indices.push_back(3);
    indices.push_back(1);
    indices.push_back(5);
    
    indices.push_back(7);
    indices.push_back(3);
    indices.push_back(5);

    indices.push_back(7);
    indices.push_back(5);
    indices.push_back(6);

    indices.push_back(7);
    indices.push_back(5);
    indices.push_back(4);

    indices.push_back(6);
    indices.push_back(0);
    indices.push_back(2);

    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(0);

    indices.push_back(6);
    indices.push_back(2);
    indices.push_back(3);

    indices.push_back(7);
    indices.push_back(6);
    indices.push_back(3);

    indices.push_back(4);
    indices.push_back(1);
    indices.push_back(0);

    indices.push_back(5);
    indices.push_back(4);
    indices.push_back(1);

    vertices.push_back({-50,0,50});
    vertices.push_back({-50,0,-50});
    vertices.push_back({50,0,-50});
    vertices.push_back({50,0,50});

    indices.push_back(8);
    indices.push_back(9);
    indices.push_back(11);

    indices.push_back(11);
    indices.push_back(9);
    indices.push_back(10);
}