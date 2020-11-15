#include "transform.h"
#include <algorithm>
#include <assert.h>

Transform::Transform(const Vec3f& pos) : Transform(pos, Quaternion::identity) {
}

Transform::Transform(const Vec3f& pos, const Quaternion& rot) :
    position(pos), 
    rotation(rot) 
{
    // local_to_world_ = Matrix4x4::TR(local_position_, local_rotation_);
    // world_to_local_ = local_to_world_;
    // world_to_local_.InverseOrthonormal();
}

//local -> world
Vec3f Transform::ApplyTransform(const Vec3f& point) const {
    Vec3f result = rotation * point;
    result += position;

    return result;
}

//world -> local
Vec3f Transform::InverseTransform(const Vec3f& point) const {
    Vec3f result = point -  position;
    return rotation.Inverse() * result;
}

//local -> world
Vec3f Transform::ApplyTransformVector(const Vec3f& dir) const {
    return rotation * dir;
}

//world -> local
Vec3f Transform::InverseTransformVector(const Vec3f& dir) const {
    return rotation.Inverse() * dir;
}
