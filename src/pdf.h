#pragma once

#include "onb.h"
#include "hittable.h"

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

    virtual double Value(const Vec3f& direction) const = 0;
    virtual Vec3f Generate() const = 0;
};

class CosinePDF : public PDF {
public:
    CosinePDF(const Vec3f& w) { uvw.BuildFromW(w); }

    virtual double Value(const Vec3f& direction) const override {
        auto cosine = direction.Normalize().Dot(uvw.w());
        return (cosine <= 0) ? 0 : cosine/math::kPI;
    }

    virtual Vec3f Generate() const override {
        return uvw.local(random_cosine_direction());
    }

public:
    ONB uvw;
};

class HittablePDF : public PDF {
public:
    HittablePDF(std::shared_ptr<Hittable> p, const Vec3f& origin) : ptr(p), o(origin) {}

    virtual double Value(const Vec3f& direction) const override {
        if (ptr)
            return ptr->PDF(o, direction);
        else
            return 0.0;
    }

    virtual Vec3f Generate() const override {
        if (ptr)
            return ptr->Sample(o);
        else
            return Vec3f(1, 0, 0);
    }

public:
    std::shared_ptr<Hittable> ptr;
    Vec3f o;
};

class MixturePDF : public PDF {
    public:
        MixturePDF(PDF* p0, PDF* p1) {
            p[0] = p0;
            p[1] = p1;
        }

        virtual double Value(const Vec3f& direction) const override {
            return 0.5 * p[0]->Value(direction) + 0.5 * p[1]->Value(direction);
        }

        virtual Vec3f Generate() const override {
            if (math::random::Random<XFloat>() < 0.5)
                return p[0]->Generate();
            else
                return p[1]->Generate();
        }

    public:
        PDF* p[2];
};

class SpherePDF : public PDF {
public:
    SpherePDF(const Vec3f& origin) : o(origin) {}

    virtual double Value(const Vec3f& direction) const override {
        return 1.0 / (4 * math::kPI);
    }

    virtual Vec3f Generate() const override {
        auto r1 = math::random::Random<XFloat>();
        auto r2 = math::random::Random<XFloat>();
        auto x = cos(2 * math::kPI * r1) * 2 * sqrt(r2 * (1-r2));
        auto y = sin(2 * math::kPI * r1) * 2 * sqrt(r2 * (1-r2));
        auto z = 1 - 2 * r2;
        return Vec3f(x, y, z);
    }

public:
    Vec3f o;
};

