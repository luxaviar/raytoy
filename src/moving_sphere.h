#pragma once

#include "hittable.h"

class MovingSphere : public Hittable {
    public:
        MovingSphere() {}

        MovingSphere(Vec3f cen0, Vec3f cen1, XFloat t0, XFloat t1, XFloat r, std::shared_ptr<Material> m)
            : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
        {
            AABB box0(center0 - radius, center0 + radius);
            AABB box1(center1 - radius, center1 + radius);
            bounding_box_ = AABB::Union(box0, box1);
        };

        virtual bool Hit(const Ray& r, XFloat tmin, XFloat tmax, HitResult& rec) const override;

        Vec3f center(XFloat t) const {
            return center0 + ((t - time0) / (time1 - time0)) * (center1 - center0);
        }

    public:
        Vec3f center0;
        Vec3f center1;
        XFloat time0;
        XFloat time1;
        XFloat radius;
        std::shared_ptr<Material> mat_ptr;
};

bool MovingSphere::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const {
    Vec3f cent = center(r.time);
    Vec3f oc = r.origin - cent;
    auto a = r.direction.MagnitudeSq();
    auto half_b = oc.Dot(r.direction);
    auto c = oc.MagnitudeSq() - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            Vec3f outward_normal = (rec.p - cent) / radius;
            rec.SetFaceNormal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.at(rec.t);
            Vec3f outward_normal = (rec.p - cent) / radius;
            rec.SetFaceNormal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }

    return false;
}
