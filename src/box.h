#pragma once

#include "hittable.h"
#include "common/transform.h"
#include "aabb.h"

class Box : public Hittable {
public:
    Box(const Vec3f& pos, const Quaternion& rot, const Vec3f& extents_, std::shared_ptr<Material> mat) : 
    transform(pos, rot), extent(extents_), mat_ptr(mat) 
    {
        const Matrix3x3& axis = transform.rotation.Inverse().ToMatrix();
        Vec3f ext = axis.r0.Abs() * extents_.x + axis.r1.Abs() * extents_.y + axis.r2.Abs() * extents_.z;
        bounding_box_ = AABB(transform.position - ext, transform.position + ext);
    }

    virtual bool Hit(const Ray& r, XFloat tmin, XFloat tmax, HitResult& rec) const override;

    Vec3f extent;
    Transform transform;
    std::shared_ptr<Material> mat_ptr;
};

bool Box::Hit(const Ray& r, XFloat tmin, XFloat tmax, HitResult& rec) const {
    AABB aabb(Vec3f(-extent.x, -extent.y, -extent.z), extent);
    Ray local_ray(transform.InverseTransform(r.origin), transform.InverseTransformVector(r.direction));

    XFloat t;
    if (!aabb.Hit(local_ray, tmin, tmax, &t)) {
        return false;
    }

    Vec3f p = local_ray.at(t);
    p += extent;

    Vec3f normal;
    if (math::AlmostEqual(p.x, 0)) {
        normal = Vec3f(-1, 0, 0);
        rec.uv = {p.z / (extent.z * 2), p.y / (extent.y * 2)};
    } else if (math::AlmostEqual(p.x, extent.x * 2)) {
        normal = Vec3f(1, 0, 0);
        rec.uv = {p.z / (extent.z * 2), p.y / (extent.y * 2)};
    } else if (math::AlmostEqual(p.y, 0)) {
        normal = Vec3f(0, -1, 0);
        rec.uv = {p.x / (extent.x * 2), p.z / (extent.z * 2)};
    } else if (math::AlmostEqual(p.y, extent.y * 2)) {
        normal = Vec3f(0, 1, 0);
        rec.uv = {p.x / (extent.x * 2), p.z / (extent.z * 2)};
    } else if (math::AlmostEqual(p.z, 0)) {
        normal = Vec3f(0, 0, -1);
        rec.uv = {p.x / (extent.x * 2), p.y / (extent.y * 2)};
    } else if (math::AlmostEqual(p.z, extent.z * 2)) {
        normal = Vec3f(0, 0, 1);
        rec.uv = {p.x / (extent.x * 2), p.y / (extent.y * 2)};
    }

    if (aabb.Contains(local_ray.origin)) {
        normal = -normal;
    }

    normal = transform.ApplyTransformVector(normal);
    rec.SetFaceNormal(r, normal);

    rec.t = t;
    rec.p = r.at(t);
    rec.mat_ptr = mat_ptr;

    return true;
}
