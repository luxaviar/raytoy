#pragma once

#include "hittable.h"

class Sphere : public Hittable {
public:
    Sphere() {}

    Sphere(Vec3f cen, XFloat r, std::shared_ptr<Material> m)
        : center(cen), radius(r), mat_ptr(m)
    {
        bounding_box_ = AABB(center - radius, center + radius);
    };

    virtual bool Hit(const Ray& r, XFloat tmin, XFloat tmax, HitResult& rec) const override;

    static Vec2f GetUV(const Vec3f& p);
public:
    Vec3f center;
    XFloat radius;
    std::shared_ptr<Material> mat_ptr;
};

Vec2f Sphere::GetUV(const Vec3f& p) {
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = acos(-p.y);
    auto phi = atan2(-p.z, p.x) + math::kPI;

    return Vec2f(phi / (2 * math::kPI), theta / math::kPI);
}

bool Sphere::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& res) const {
    Vec3f oc = r.origin - center;
    auto a = r.direction.MagnitudeSq();
    auto half_b = oc.Dot(r.direction);
    auto c = oc.MagnitudeSq() - radius*radius;
    auto discriminant = half_b*half_b - a*c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            res.t = temp;
            res.p = r.at(res.t);
            Vec3f outward_normal = (res.p - center) / radius;
            res.SetFaceNormal(r, outward_normal);
            res.uv = GetUV(outward_normal);
            res.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            res.t = temp;
            res.p = r.at(res.t);
            Vec3f outward_normal = (res.p - center) / radius;
            res.SetFaceNormal(r, outward_normal);
            res.uv = GetUV(outward_normal);
            res.mat_ptr = mat_ptr;
            return true;
        }
    }

    return false;
}
