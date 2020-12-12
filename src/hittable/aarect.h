#pragma once

#include <algorithm>
#include <type_traits>
#include "math/vec3.h"
#include "common/ray.h"
#include "hittable.h"
#include "math/axis.h"

template<math::Axis axis, bool face_positive=true>
class AARect : public Hittable {
public:
    AARect() {}

    AARect(XFloat _x0, XFloat _x1, XFloat _y0, XFloat _y1, XFloat _k, std::shared_ptr<Material> mat) : 
        Hittable(mat), x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k)
    {
        ik = static_cast<typename std::underlying_type<math::Axis>::type>(axis);
        if (ik == 0) {
            ix = 1;
            iy = 2;
        } else if (ik == 1) {
            ix = 0;
            iy = 2;
        } else {
            ix = 0;
            iy = 1;
        }
        Vec3f bmin;
        Vec3f bmax;

        bmin[ik] = k - 0.0001;
        bmin[ix] = x0;
        bmin[iy] = y0;

        bmax[ik] = k + 0.0001;
        bmax[ix] = x1;
        bmax[iy] = y1;

        bounding_box_ = AABB(bmin, bmax);
    };

    virtual bool Hit(const Ray& r, XFloat t0, XFloat t1, HitResult& rec) const override;
    virtual double PDF(const Vec3f& origin, const Vec3f& v) const override;
    virtual Vec3f Sample(const Vec3f& origin) const override;

public:
    int ix, iy, ik;
    XFloat x0, x1;
    XFloat y0, y1;
    XFloat k;
};

template<math::Axis axis, bool face_positive>
bool AARect<axis, face_positive>::Hit(const Ray& r, XFloat t0, XFloat t1, HitResult& rec) const {
    auto t = (k - r.origin[ik]) / r.direction[ik];
    if (t < t0 || t > t1)
        return false;

    auto x = r.origin[ix] + t*r.direction[ix];
    auto y = r.origin[iy] + t*r.direction[iy];
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;

    rec.uv.u = (x-x0)/(x1-x0);
    rec.uv.v = (y-y0)/(y1-y0);
    rec.t = t;

    Vec3f outward_normal(0.0);
    outward_normal[ik] = face_positive ? 1 : -1;
    rec.SetFaceNormal(r, outward_normal);
    rec.mat_ptr = mat_ptr_.get();
    rec.p = r.at(t);

    return true;
}

template<math::Axis axis, bool face_positive>
XFloat AARect<axis, face_positive>::PDF(const Vec3f& origin, const Vec3f& wo) const {
    HitResult rec;
    if (!Hit(Ray(origin, wo), 0.001, math::kInfinite, rec))
        return 0;

    auto area = (x1-x0) * (y1-y0);
    auto distance_squared = rec.t * rec.t * wo.MagnitudeSq();
    auto cosine = fabs(wo.Normalize().Dot(rec.normal));

    return distance_squared / (cosine * area);
}

template<math::Axis axis, bool face_positive>
Vec3f AARect<axis, face_positive>::Sample(const Vec3f& origin) const {
    XFloat r1 = math::random::Random(x0,x1);
    XFloat r2 = math::random::Random(y0,y1);
    auto random_point = Vec3f(k);

    if (axis == math::Axis::kX) {
        random_point.y = r1;
        random_point.z = r2;
    } else if (axis == math::Axis::kY) {
        random_point.x = r1;
        random_point.z = r2;
    } else {
        random_point.x = r1;
        random_point.y = r2;
    }

    return (random_point - origin).Normalize();
}
