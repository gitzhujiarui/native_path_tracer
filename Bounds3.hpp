#ifndef RAYTRACING_BOUNDS3_H
#define RAYTRACING_BOUNDS3_H

#include "Ray.hpp"
#include "Vector.hpp"
#include <limits>
#include <array>

class Bounds3 {
public:
    Vector3f pMin, pMax; // two points to specify the bounding box
    Bounds3() {
        double minNum = std::numeric_limits<double>::lowest();
        double maxNum = std::numeric_limits<double>::max();
        pMax = Vector3f(minNum, minNum, minNum);
        pMin = Vector3f(maxNum, maxNum, maxNum);
    }

    Bounds3(const Vector3f p) : pMin(p), pMax(p) {}

    Bounds3(const Vector3f p1, const Vector3f p2) {
        pMin = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
        pMax = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
    }

    Vector3f Diagonal() const { return pMax - pMin; }

    int maxExtent() const {
        Vector3f d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    double SurfaceArea() const {
        Vector3f d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    Vector3f Centroid() { return 0.5 * pMin + 0.5 * pMax; }

    Bounds3 Intersect(const Bounds3 &b) {
        return Bounds3(Vector3f(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y),
                                fmax(pMin.z, b.pMin.z)),
                       Vector3f(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y),
                                fmin(pMax.z, b.pMax.z)));
    }

    Vector3f Offset(const Vector3f &p) const {
        Vector3f o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;
        return o;
    }

    bool Overlaps(const Bounds3 &b1, const Bounds3 &b2) {
        bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
        bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
        bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
        return (x && y && z);
    }

    bool Inside(const Vector3f &p, const Bounds3 &b) {
        return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
                p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
    }

    inline const Vector3f &operator[](int i) const {
        return (i == 0) ? pMin : pMax;
    }

    inline bool IntersectP(const Ray &ray, const Vector3f &invDir,
                           const std::array<int, 3> &dirisNeg) const;
};


inline bool Bounds3::IntersectP(const Ray &ray, const Vector3f &invDir,
                                const std::array<int, 3> &dirIsNeg) const {
    float tx_min, tx_max, ty_min, ty_max, tz_min, tz_max;
    if (!dirIsNeg[0]) {
        tx_min = (this->pMin.x - ray.origin.x) * invDir.x;
        tx_max = (this->pMax.x - ray.origin.x) * invDir.x;
    } else {
        tx_min = (this->pMax.x - ray.origin.x) * invDir.x;
        tx_max = (this->pMin.x - ray.origin.x) * invDir.x;
    }
    if (!dirIsNeg[1]) {
        ty_min = (this->pMin.y - ray.origin.y) * invDir.y;
        ty_max = (this->pMax.y - ray.origin.y) * invDir.y;
    } else {
        ty_min = (this->pMax.y - ray.origin.y) * invDir.y;
        ty_max = (this->pMin.y - ray.origin.y) * invDir.y;
    }
    if (!dirIsNeg[2]) {
        tz_min = (this->pMin.z - ray.origin.z) * invDir.z;
        tz_max = (this->pMax.z - ray.origin.z) * invDir.z;
    } else {
        tz_min = (this->pMax.z - ray.origin.z) * invDir.z;
        tz_max = (this->pMin.z - ray.origin.z) * invDir.z;
    }
    auto t_enter = std::max(tx_min, std::max(ty_min, tz_min));
    auto t_exit = std::min(tx_max, std::min(ty_max, tz_max));
    return t_enter < t_exit + 1e-4 && t_exit > 0;
}

inline Bounds3 Union(const Bounds3 &b1, const Bounds3 &b2) {
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b1.pMin, b2.pMin);
    ret.pMax = Vector3f::Max(b1.pMax, b2.pMax);
    return ret;
}

inline Bounds3 Union(const Bounds3 &b, const Vector3f &p) {
    Bounds3 ret;
    ret.pMin = Vector3f::Min(b.pMin, p);
    ret.pMax = Vector3f::Max(b.pMax, p);
    return ret;
}

#endif // RAYTRACING_BOUNDS3_H
