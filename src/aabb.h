#pragma once

#include <algorithm>
#include "math/vec3.h"
#include "ray.h"

struct AABB {
    AABB() : min(Vec3f::zero), max(Vec3f::zero) {}
    AABB(const Vec3f& a, const Vec3f& b) : min(a), max(b) {}

    bool Hit(const Ray& r, XFloat t_min, XFloat t_max, XFloat* t=nullptr) const {
        XFloat t_enter = -math::kInfinite;
        XFloat t_exit = math::kInfinite;

        for (int i = 0; i < 3; i++) {
            auto invD = 1.0f / r.direction[i];
            auto t0 = (min[i] - r.origin[i]) * invD;
            auto t1 = (max[i] - r.origin[i]) * invD;
            if (invD < 0.0)
                std::swap(t0, t1);
            if (t0 > t_enter) t_enter = t0;
            if (t1 < t_exit) t_exit = t1;
            if (t_exit <= t_enter || t_enter > t_max || t_exit < t_min)
                return false;
        }

        if (t) {
            if (t_enter >= t_min && t_enter <= t_max)
                *t = t_enter;
            else 
                *t = t_exit;
        }

        return true;
    }

    XFloat Area() const {
        auto a = max.x - min.x;
        auto b = max.y - min.y;
        auto c = max.z - min.z;
        return 2*(a*b + b*c + c*a);
    }

    int LongestAxis() const {
        auto a = max.x - min.x;
        auto b = max.y - min.y;
        auto c = max.z - min.z;
        if (a > b && a > c)
            return 0;
        else if (b > c)
            return 1;
        else
            return 2;
    }

    bool Compare(const AABB& other, int axis) const {
        return min[axis] < other.min[axis];
    }

    bool Contains(const Vec3f& point) const {
        return point.x >= min.x && point.x <= max.x &&
            point.y >= min.y && point.y <= max.y &&
            point.z >= min.z && point.z <= max.z;
    }

    static AABB Union(const AABB& left, const AABB& right) {
        Vec3f mi(fmin(left.min.x, right.min.x),
                fmin(left.min.y, right.min.y),
                fmin(left.min.z, right.min.z));

        Vec3f ma(fmax(left.max.x, right.max.x),
                fmax(left.max.y, right.max.y),
                fmax(left.max.z, right.max.z));
        
        return AABB(mi, ma);
    }

    Vec3f min;
    Vec3f max;
};
