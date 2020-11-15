#pragma once

#include "math/vec3.h"
#include "math/quat.h"
// #include "math/mat4.h"

struct Transform {
    Transform(const Vec3f& pos);
    Transform(const Vec3f& pos, const Quaternion& rot);

    //local -> world
    Vec3f ApplyTransform(const Vec3f& point) const;
    //world -> local
    Vec3f InverseTransform(const Vec3f& point) const;
    //local -> world
    Vec3f ApplyTransformVector(const Vec3f& dir) const;
    //world -> local
    Vec3f InverseTransformVector(const Vec3f& dir) const;

    Vec3f forward() const { return rotation * Vec3f::forward; }
    Vec3f right() const { return rotation * Vec3f::right; }
    Vec3f up() const { return rotation * Vec3f::up; }

    Vec3f position;
    Quaternion rotation;
};
