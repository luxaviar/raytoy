#pragma once

#include <memory>
#include <vector>
#include "ray.h"
#include "aabb.h"
#include "math/vec2.h"

class Material;

struct HitResult {
    Vec3f p;
    Vec3f normal;
    Material* mat_ptr;
    XFloat t;
    Vec2f uv;
    bool front_face;

    inline void SetFaceNormal(const Ray& r, const Vec3f& outward_normal) {
        front_face = r.direction.Dot(outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class Hittable : public std::enable_shared_from_this<Hittable> {
public:
    Hittable() {}
    Hittable(std::shared_ptr<Material> m) : mat_ptr_(m) { }

    virtual bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const = 0;

    virtual XFloat PDF(const Vec3f& o, const Vec3f& v) const {
        return 0.0;
    }

    virtual Vec3f Sample(const Vec3f& o) const {
        return Vec3f(1,0,0);
    }

    const AABB& bounding_box() const {
        return bounding_box_;
    };

    virtual void FetchLight(std::vector<std::shared_ptr<Hittable>>& lights);
    virtual void BuildBVH() {};

protected:
    std::shared_ptr<Material> mat_ptr_;
    AABB bounding_box_;
};
