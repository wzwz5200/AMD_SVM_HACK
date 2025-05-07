#ifndef MATH_HPP
#define MATH_HPP

#include <algorithm>

#include <cmath>
#include <initializer_list>

#undef min
#undef max
#include <limits>
struct VCVector3 {
    float x, y, z;

    VCVector3() : x(0), y(0), z(0) {}
    VCVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    VCVector3 operator-(const VCVector3& other) const {
        return VCVector3(x - other.x, y - other.y, z - other.z);
    }

    float dot(const VCVector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    VCVector3 cross(const VCVector3& other) const {
        return VCVector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

struct Triangle {
    int a, b, c;
    Triangle() : a(0), b(0), c(0) {}
    Triangle(int a_, int b_, int c_) : a(a_), b(b_), c(c_) {}
};

struct AABB {
    VCVector3 min;
    VCVector3 max;

    bool RayIntersects(const VCVector3& rayOrigin, const VCVector3& rayDir) const {
        float tmin = std::numeric_limits<float>::lowest();
        float tmax = std::numeric_limits<float>::max();

        const float* rayOriginArr = &rayOrigin.x;
        const float* rayDirArr = &rayDir.x;
        const float* minArr = &min.x;
        const float* maxArr = &max.x;

        for (int i = 0; i < 3; ++i) {
            float invDir = 1.0f / rayDirArr[i];
            float t0 = (minArr[i] - rayOriginArr[i]) * invDir;
            float t1 = (maxArr[i] - rayOriginArr[i]) * invDir;

            if (invDir < 0.0f) std::swap(t0, t1);
            tmin = std::max(tmin, t0);
            tmax = std::min(tmax, t1);
        }

        return tmax >= tmin && tmax >= 0;
    }
};

struct TriangleCombined {
    VCVector3 v0, v1, v2;

    TriangleCombined() = default;
    TriangleCombined(const VCVector3& v0_, const VCVector3& v1_, const VCVector3& v2_)
        : v0(v0_), v1(v1_), v2(v2_) {
    }

    AABB ComputeAABB() const {
        VCVector3 min_point, max_point;

        min_point.x = std::min({ v0.x, v1.x, v2.x });
        min_point.y = std::min({ v0.y, v1.y, v2.y });
        min_point.z = std::min({ v0.z, v1.z, v2.z });

        max_point.x = std::max({ v0.x, v1.x, v2.x });
        max_point.y = std::max({ v0.y, v1.y, v2.y });
        max_point.z = std::max({ v0.z, v1.z, v2.z });

        return { min_point, max_point };
    }
};

#endif
