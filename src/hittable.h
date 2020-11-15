#pragma once

#include <memory>
#include "ray.h"
#include "aabb.h"
#include "math/vec2.h"

class Material;

struct HitResult {
    Vec3f p;
    Vec3f normal;
    std::shared_ptr<Material> mat_ptr;
    XFloat t;
    Vec2f uv;
    bool front_face;

    inline void SetFaceNormal(const Ray& r, const Vec3f& outward_normal) {
        front_face = r.direction.Dot(outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class Hittable {
public:
    virtual bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const = 0;

    const AABB& bounding_box() const {
        return bounding_box_;
    };

protected:
    AABB bounding_box_;
};
