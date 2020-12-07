#pragma once

#include "math/random.h"

struct ONB {
    inline Vec3f operator[](int i) const { return axis[i]; }

    const Vec3f& u() const { return axis[0]; }
    const Vec3f& v() const { return axis[1]; }
    const Vec3f& w() const { return axis[2]; }

    Vec3f local(double a, double b, double c) const {
        return a * u() + b * v() + c * w();
    }

    Vec3f local(const Vec3f& a) const {
        return a.x * u() + a.y * v() + a.z * w();
    }

    void BuildFromW(const Vec3f& n) {
        axis[2] = n.Normalize();
        Vec3f a = (fabs(w().x) > 0.9) ? Vec3f(0,1,0) : Vec3f(1,0,0);
        axis[1] = w().Cross(a).Normalize();
        axis[0] = w().Cross(v());
    }

    Vec3f axis[3];
};
