#pragma once

#include "ray.h"
#include "hittable.h"
#include "vertex.h"

class Triangle : public Hittable {
public:
    static void Barycentric(const Vec3f& p, const Vec3f& a, const Vec3f& b, const Vec3f& c, XFloat& u, XFloat& v, XFloat& w);

    Triangle(const Vertex& a, const Vertex& b, const Vertex& c, std::shared_ptr<Material> mat, bool interpolate_normal_=true);

    bool Intersects(const Ray& ray, XFloat& t) const;
    bool Intersects(const Ray& ray, XFloat t_min, XFloat t_max, XFloat& t) const;

    bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const override;
    XFloat PDF(const Vec3f& o, const Vec3f& v) const override;
    Vec3f Sample(const Vec3f& o) const override;

    //CW order
    Vertex v0;
    Vertex v1;
    Vertex v2;
    Vec3f normal;
    XFloat area;
    bool interpolate_normal;
};
