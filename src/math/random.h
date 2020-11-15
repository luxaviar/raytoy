#pragma once

#include <cstdint>
#include "rand.h"
#include "vec2.h"
#include "vec3.h"
#include "quat.h"
#include "util.h"

namespace math {
namespace random {

extern math::Rand g_rng;

inline void SetSeed(uint32_t sd) {
    g_rng.SetSeed(sd);
}

inline uint32_t Value() {
    return g_rng.Get();
}

inline int Random(int min, int max) {
    int dif;
    if (min < max) {
        dif = max - min;
        int t = g_rng.Get() % dif;
        t += min;
        return t;
    } else if (min > max) {
        dif = min - max;
        int t = g_rng.Get() % dif;
        t = min - t;
        return t;
    } else {
        return min;
    }        
}

template<typename T>
inline T Random() {
    return (T)g_rng.GetFloat();
}

template<typename T>
inline T Random(T min, T max) {
    T t = Random<T>();
    t = min * t + (1.0 - t) * max;
    return t;
}

inline Vec3f Vector() {
    return Vec3f(Random<XFloat>(), Random<XFloat>(), Random<XFloat>());
}

inline Vec3f Vector(XFloat min, XFloat max) {
    return Vec3f(Random<XFloat>(min, max), Random<XFloat>(min, max), Random<XFloat>(min, max));
}

inline Vec3f UnitVector() {
    XFloat z = Random<XFloat>(-1.0, 1.0);
    XFloat a = Random<XFloat>(0.0, 2.0 * kPI);

    XFloat r = ::sqrt (1.0 - z*z);

    XFloat x = r * ::cos (a);
    XFloat y = r * ::sin (a);

    return Vec3f(x, y, z);
}

inline Vec2f UnitVec2f() {
    XFloat a = Random<XFloat>(0.0, 2.0 * kPI);

    XFloat x = ::cos(a);
    XFloat y = ::sin(a);

    return Vec2f(x, y);
}

inline ::Quaternion Quaternion() {
    ::Quaternion q;
    q.x = Random<XFloat>(-1.0, 1.0);
    q.y = Random<XFloat>(-1.0, 1.0);
    q.z = Random<XFloat>(-1.0, 1.0);
    q.w = Random<XFloat>(-1.0, 1.0);
    q.Normalized();
    if (q.Dot(Quaternion::identity) < 0.0)
        return -q;
    else
        return q;
}

inline Vec3f PointInsideUnitSphere() {
    Vec3f v = UnitVector();
    v *= Pow(Random<XFloat>(), 1.0 / 3.0);
    return v;
}

inline Vec2f PointInsideUnitCircle() {
    Vec2f v = UnitVec2f();
    // As the volume of the sphere increases (x^3) over an interval we have to increase range as well with x^(1/3)
    v *= Pow(Random<XFloat>(0.0, 1.0), 1.0 / 2.0);
    return v;
}

}
}