#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"
#include "math/util.h"

class ConstantMedium : public Hittable  {
    public:
        ConstantMedium(std::shared_ptr<Hittable> b, XFloat d, std::shared_ptr<Texture> a)
            : boundary(b),
              neg_inv_density(-1/d),
              phase_function(std::make_shared<Isotropic>(a))
            {
                bounding_box_ = boundary->bounding_box();
            }

        ConstantMedium(std::shared_ptr<Hittable> b, XFloat d, Color c)
            : boundary(b),
              neg_inv_density(-1/d),
              phase_function(std::make_shared<Isotropic>(c))
            {
                bounding_box_ = boundary->bounding_box();
            }

        virtual bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const override;

    public:
        std::shared_ptr<Hittable> boundary;
        std::shared_ptr<Material> phase_function;
        XFloat neg_inv_density;
};

bool ConstantMedium::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    // const bool enableDebug = false;
    // const bool debugging = enableDebug && math::random::Random<XFloat>() < 0.00001;

    HitResult rec1, rec2;

    if (!boundary->Hit(r, -math::kInfinite, math::kInfinite, rec1))
        return false;

    if (!boundary->Hit(r, rec1.t+0.0001, math::kInfinite, rec2))
        return false;

    // if (debugging) std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

    if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    const auto ray_length = r.direction.Magnitude();
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const auto rd = math::random::Random<XFloat>();
    const auto hit_distance = neg_inv_density * std::log(rd);

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = Vec3f(1,0,0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat_ptr = phase_function.get();

    return true;
}
