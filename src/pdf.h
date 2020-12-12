#pragma once

#include "onb.h"
#include "hittable/hittable.h"
#include "common/ray.h"

inline Vec3f random_cosine_direction() {
    auto r1 = math::random::Random<XFloat>();
    auto r2 = math::random::Random<XFloat>();
    auto z = sqrt(1-r2);

    auto phi = 2 * math::kPI * r1;
    auto x = cos(phi) * sqrt(r2);
    auto y = sin(phi) * sqrt(r2);

    return Vec3f(x, y, z);
}

inline Vec3f random_to_sphere(double radius, double distance_squared) {
    auto r1 = math::random::Random<XFloat>();
    auto r2 = math::random::Random<XFloat>();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto phi = 2 * math::kPI * r1;
    auto x = cos(phi) * sqrt(1 - z * z);
    auto y = sin(phi) * sqrt(1 - z * z);

    return Vec3f(x, y, z);
}

class PDF  {
public:
    virtual ~PDF() {}

    virtual double Value(const HitResult& res, const Vec3f& direction) const = 0;
    virtual Vec3f Sample(const HitResult& res, XFloat& pdf) const = 0;
};

class CosinePDF : public PDF {
public:
    CosinePDF() { }

    virtual double Value(const HitResult& res, const Vec3f& direction) const override {
        auto cosine = direction.Dot(res.normal);
        return (cosine <= 0) ? 0 : cosine/math::kPI;
    }

    virtual Vec3f Sample(const HitResult& res, XFloat& pdf) const {
        ONB uvw;
        uvw.BuildFromW(res.normal);
        auto wo = uvw.local(random_cosine_direction());
        pdf = Value(res, wo);
        return wo;
    }
};

class HittablePDF : public PDF {
public:
    HittablePDF(std::shared_ptr<Hittable> p) : ptr(p) {}

    virtual double Value(const HitResult& res, const Vec3f& direction) const override {
        return ptr->PDF(res.p, direction);
    }

    virtual Vec3f Sample(const HitResult& res, XFloat& pdf) const {
        auto wo = ptr->Sample(res.p);
        pdf = Value(res, wo);
        return wo;
    }

public:
    std::shared_ptr<Hittable> ptr;
};

class MixturePDF : public PDF {
    public:
        MixturePDF(PDF* p0, PDF* p1) {
            p[0] = p0;
            p[1] = p1;
        }

        virtual double Value(const HitResult& res, const Vec3f& direction) const override {
            return 0.5 * p[0]->Value(res, direction) + 0.5 * p[1]->Value(res, direction);
        }

        virtual Vec3f Sample(const HitResult& res, XFloat& pdf) const {
            Vec3f wo;
            if (math::random::Random<XFloat>() < 0.5) {
                wo = p[0]->Sample(res, pdf);
            } else {
                wo = p[1]->Sample(res, pdf);;
            }
            
            pdf = Value(res, wo);

            return wo;
        }

    public:
        PDF* p[2];
};

class SphericalPDF : public PDF {
public:
    SphericalPDF() {}

    virtual double Value(const HitResult& res, const Vec3f& direction) const override {
        return 1.0 / (4 * math::kPI);
    }

    virtual Vec3f Sample(const HitResult& res, XFloat& pdf) const {
        auto r1 = math::random::Random<XFloat>();
        auto r2 = math::random::Random<XFloat>();
        auto x = cos(2 * math::kPI * r1) * 2 * sqrt(r2 * (1-r2));
        auto y = sin(2 * math::kPI * r1) * 2 * sqrt(r2 * (1-r2));
        auto z = 1 - 2 * r2;
        auto wo = Vec3f(x, y, z);
        pdf = Value(res, wo);
        return wo;
    }
};

