#pragma once

#include <algorithm>
#include <type_traits>
#include "math/vec3.h"
#include "ray.h"
#include "hittable.h"
#include "math/axis.h"

template<math::Axis axis>
class AARect : public Hittable {
public:
    AARect() {}

    AARect(XFloat _x0, XFloat _x1, XFloat _y0, XFloat _y1, XFloat _k, std::shared_ptr<Material> mat) : 
        x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) 
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

public:
    std::shared_ptr<Material> mp;
    int ix, iy, ik;
    XFloat x0, x1;
    XFloat y0, y1;
    XFloat k;
};

template<math::Axis axis>
bool AARect<axis>::Hit(const Ray& r, XFloat t0, XFloat t1, HitResult& rec) const {
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
    outward_normal[ik] = 1;
    rec.SetFaceNormal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);

    return true;
}

