
#include "Scene.hpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include <chrono>
#include <queue>

#pragma once
struct hit_payload
{
    float tNear;
    uint32_t index;
    Vector2f uv;
    Object* hit_obj;
};

class Renderer
{
public:
    Renderer(std::vector<std::vector<double>>& v);
    void Render(const Scene& scene);
    std::vector<double> getSobolRandom();
    std::vector<std::vector<double>> sobol_sequence;
    int sobol_sequence_counter;

private:
};
