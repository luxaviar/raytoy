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

    virtual double PDFValue(const Vec3f& o, const Vec3f& v) const {
        return 0.0;
    }

    virtual Vec3f Random(const Vec3f& o) const {
        return Vec3f(1,0,0);
    }

    const AABB& bounding_box() const {
        return bounding_box_;
    };

protected:
    AABB bounding_box_;
};

class FlipFace : public Hittable {
public:
    FlipFace(std::shared_ptr<Hittable> p) : ptr(p) {
        bounding_box_ = p->bounding_box();
    }

    virtual bool Hit(const Ray& r, double t_min, double t_max, HitResult& rec) const override {
        if (!ptr->Hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

public:
    std::shared_ptr<Hittable> ptr;
};
