#include "triangle.h"
#include <limits>
#include <math.h>
#include "math/vec3.h"
#include "math/random.h"

void Triangle::Barycentric(const Vec3f& p, const Vec3f& a, const Vec3f& b, const Vec3f& c, XFloat& u, XFloat& v, XFloat& w) {
    // code from Crister Erickson's Real-Time Collision Detection
    Vec3f v0 = b - a;
    Vec3f v1 = c - a;
    Vec3f v2 = p - a;

    Vec3f::value_type d00 = v0.Dot(v0);
    Vec3f::value_type d01 = v0.Dot(v1);
    Vec3f::value_type d11 = v1.Dot(v1);
    Vec3f::value_type d20 = v2.Dot(v0);
    Vec3f::value_type d21 = v2.Dot(v1);
    Vec3f::value_type invDenom = 1.0f / (d00 * d11 - d01 * d01);

    v = (d11 * d20 - d01 * d21) * invDenom;
    w = (d00 * d21 - d01 * d20) * invDenom;
    u = 1.0f - v - w;
}

Triangle::Triangle(const Vertex& a, const Vertex& b, const Vertex& c, std::shared_ptr<Material> mat, bool interpolate_normal_) :
    Hittable(mat), v0(a), v1(b), v2(c), interpolate_normal(interpolate_normal_)
{
    auto e1 = v1.position - v0.position;
    auto e2 = v2.position - v0.position;
    normal = e1.Cross(e2);
    area = normal.Magnitude() * 0.5f;
    normal.Normalized();

    Vec3f min = Vec3f::Min(v0.position, Vec3f::Min(v1.position, v2.position));
    Vec3f max = Vec3f::Max(v0.position, Vec3f::Max(v1.position, v2.position));
    for (int i = 0; i < 3; ++i) {
        if (math::Abs(max[i] - min[i]) < math::kEpsilon) {
            min[i] -= 0.0001;
            max[i] += 0.0001;
        }
    }
    
    bounding_box_ = AABB(min, max);
}

bool Triangle::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const {
    XFloat t;
    bool hit = Intersects(r, t_min, t_max, t);
    if (!hit) {
        return false;
    }

    rec.p = r.at(t);
    XFloat w0, w1, w2;
    Barycentric(rec.p, v0.position, v1.position, v2.position, w0, w1, w2);
    Vec3f outward_normal = normal;
    if (interpolate_normal) {
        outward_normal = v0.normal * w0 + v1.normal * w1 + v2.normal * w2;
    }

    rec.SetFaceNormal(r, outward_normal);
    rec.uv = v0.texcoord * w0 + v1.texcoord * w1 + v2.texcoord * w2;
    rec.mat_ptr = mat_ptr_.get();
    rec.t = t;

    return true;
}

XFloat Triangle::PDF(const Vec3f& o, const Vec3f& v) const {
    HitResult rec;
    if (!Hit(Ray(o, v), 0.0, math::kInfinite, rec)) {
        return 0.0;
    }
    
    const XFloat dist2 = (rec.p - o).MagnitudeSq();
    const XFloat cosine = math::Abs(v.Dot(normal));

    return dist2 / (cosine * area);
}

// see Osada et All, Shape Distributions, section 4.2
// http://www.cs.princeton.edu/~funk/tog02.pdf
Vec3f Triangle::Sample(const Vec3f& o) const {
    XFloat x = std::sqrt(math::random::Random<XFloat>());
    XFloat y = math::random::Random<XFloat>();
    auto point = v0.position * (1.0 - x) + v1.position * (x * (1.0 - y)) + v2.position * (x * y);
    return (point - o).Normalize();
}

bool Triangle::Intersects(const Ray& ray, XFloat& t) const {
    return Intersects(ray, 0.0, std::numeric_limits<XFloat>::max(), t);
}

// Fast, minimum storage ray-triangle intersection.
// Tomas Möller and Ben Trumbore. 
// Journal of Graphics Tools, 2(1):21--28, 1997. 
// http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
bool Triangle::Intersects(const Ray& ray, XFloat t_min, XFloat t_max, XFloat& t) const {
    // Edge vectors
    Vec3f e1 = v1.position - v0.position;
    Vec3f e2 = v2.position - v0.position;

    // begin calculating determinant - also used to calculate U parameter
    Vec3f pvec = ray.direction.Cross(e2);

    // If det < 0, intersecting backfacing tri, > 0, intersecting frontfacing tri, 0, parallel to plane.
    const float det = e1.Dot(pvec);

    // If determinant is near zero, ray lies in plane of triangle.
    if (det > -math::kEpsilon && det < math::kEpsilon)
        return false;

    const float inv_det = 1.f / det;

    // Calculate distance from v0 to ray origin
    Vec3f tvec = ray.origin - v0.position;

    // Output barycentric u
    XFloat u = tvec.Dot(pvec) * inv_det;
    if (u < 0.0 || u > 1.0)
        return false; // Barycentric U is outside the triangle - early out.

    // Prepare to test V parameter
    Vec3f qvec = tvec.Cross(e1);

    // Output barycentric v
    XFloat v = ray.direction.Dot(qvec) * inv_det;
    if (v < 0 || u + v > 1.0) // Barycentric V or the combination of U and V are outside the triangle - no intersection.
        return false;

    // Barycentric u and v are in limits, the ray intersects the triangle.
    
    // Output signed distance from ray to triangle.
    t = e2.Dot(qvec) * inv_det;
    if (t < t_min || t > t_max) {
        return false;
    }
    
    return true;
//	return (det < 0.f) ? IntersectBackface : IntersectFrontface;
}
