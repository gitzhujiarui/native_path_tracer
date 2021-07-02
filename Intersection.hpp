#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false;
        coords=Vector3f();
        normal=Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened;      // whether the intersected happened
    Vector3f coords;    // coordinates of hit position
    Vector3f tcoords;
    Vector3f normal;    // normal on hit position
    Vector3f emit;      // emission on hit position
    double distance;    
    Object* obj;        // object on hit position
    Material* m;        // material on hit position
};
#endif //RAYTRACING_INTERSECTION_H
