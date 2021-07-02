#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
#include <chrono>
#include <queue>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
void getSobolSequence(std::vector<std::vector<double>>& v, std::string filename) {
    std::fstream sobol;
    sobol.open(filename);
    if (!sobol.is_open()) {
       std::cerr << "open error\n";
    }
    std::string line;
    while (getline(sobol, line)) {
        std::string line_value;
        std::stringstream ss(line);
        std::vector<double> temp;
        while (std::getline(ss, line_value, ',')) {
            std::istringstream os(line_value);
            double d;
            os >> d;
            temp.push_back(d);
        }
        v.push_back(temp);
    }
}

int main(int argc, char **argv) {

    // first get the sobol_sequence from the file
    std::vector<std::vector<double>> sobol_sequence;
    std::string filename = "sobol_seq.csv";
    getSobolSequence(sobol_sequence,filename);
    for (auto v : sobol_sequence) {
        std::cout << v[0] << "      " << v[1] << std::endl;
    }
    std::cout << "Successfully get sobol sequence from file" << std::endl;
    std::cout << "Print something" << std::endl;

    Scene scene(512, 512);

    Material *red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.63f, 0.065f, 0.05f);
    Material *green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.14f, 0.45f, 0.091f);
    Material *white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.725f, 0.71f, 0.68f);
    Material *light = new Material(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) +
                                             15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) +
                                             18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
    light->Kd = Vector3f(0.65f);
    Material *fun_color = new Material(SPECULAR, Vector3f(0.0f));
    fun_color->Kd = Vector3f(1,1,1)*0.999;

    MeshTriangle floor("../models/cornellbox/floor.obj", white);
    MeshTriangle shortbox("../models/cornellbox/shortbox.obj", white);
    MeshTriangle tallbox("../models/cornellbox/tallbox.obj", white);
    // MeshTriangle tallbox("../models/cornellbox/tallbox.obj", fun_color);
    MeshTriangle left("../models/cornellbox/left.obj", red);
    MeshTriangle right("../models/cornellbox/right.obj", green);
    MeshTriangle light_("../models/cornellbox/light.obj", light);

    // MeshTriangle original("../models/cornellBox_2/CornellBox-Original.obj", red);
    // scene.Add(&original);


    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);
    // scene.Add(dynamic_cast<Object*>(new Sphere(Vector3f(73,16.5,78), 32, fun_color)));


    scene.buildBVH();

    Renderer r(sobol_sequence);

    auto start = std::chrono::system_clock::now();
    r.Render(scene);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count()
              << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";

    return 0;
}