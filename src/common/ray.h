#pragma once

#include "math/vec3.h"

struct Ray {
    Ray() {}
    Ray(const Vec3f& origin_, const Vec3f& direction_)
        : origin(origin_), direction(direction_), time(0)
    {}

    Ray(const Vec3f& origin_, const Vec3f& direction_, XFloat time_)
        : origin(origin_), direction(direction_), time(time_)
    {}

    Vec3f at(XFloat t) const {
        return origin + t * direction;
    }

    Vec3f origin;
    Vec3f direction;
    XFloat time;
};
