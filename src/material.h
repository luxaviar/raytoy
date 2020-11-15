#pragma once

#include "ray.h"
#include "hittable.h"
#include "math/random.h"
#include "texture.h"

class Material {
public:
    virtual bool Scatter(const Ray& r_in, const HitResult& rec, Color& attenuation, Ray& scattered) const = 0;
    virtual Color Emitted(XFloat u, XFloat v, const Vec3f& p) const {
        return Color::zero;
    }
};

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(std::make_shared<SolidColor>(a)) {}
    Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, Color& attenuation, Ray& scattered) const override {
        Vec3f scatter_direction = rec.normal + math::random::UnitVector();// random_unit_vector();
        scattered = Ray(rec.p, scatter_direction, r_in.time);
        attenuation = albedo->Value(rec.uv.u, rec.uv.v, rec.p);
        return true;
    }

public:
    std::shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
    Metal(const Color& a, XFloat f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, Color& attenuation, Ray& scattered) const override {
        Vec3f reflected = Vec3f::Reflect(r_in.direction.Normalize(), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz* math::random::Vector(), r_in.time);
        attenuation = albedo;
        return scattered.direction.Dot(rec.normal) > 0;
    }

public:
    Color albedo;
    XFloat fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(XFloat index_of_refraction) : ir(index_of_refraction) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, Color& attenuation, Ray& scattered) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        Vec3f unit_direction = r_in.direction.Normalize();
        XFloat cos_theta = fmin(-unit_direction.Dot(rec.normal), 1.0);
        XFloat sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3f direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > math::random::Random<XFloat>())
            direction = Vec3f::Reflect(unit_direction, rec.normal);
        else
            direction = Vec3f::Refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Ray(rec.p, direction, r_in.time);
        return true;
    }

public:
    XFloat ir; // Index of Refraction

private:
    static double reflectance(XFloat cosine, XFloat ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
};

class DiffuseLight : public Material {
public:
    DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
    DiffuseLight(Color c) : emit(std::make_shared<SolidColor>(c)) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, Color& attenuation, Ray& scattered) const override {
        return false;
    }

    virtual Color Emitted(XFloat u, XFloat v, const Vec3f& p) const override {
        return emit->Value(u, v, p);
    }

public:
    std::shared_ptr<Texture> emit;
};

class Isotropic : public Material {
public:
    Isotropic(Color c) : albedo(std::make_shared<SolidColor>(c)) {}
    Isotropic(std::shared_ptr<Texture> a) : albedo(a) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, Color& attenuation, Ray& scattered) const override {
        scattered = Ray(rec.p, math::random::UnitVector(), r_in.time);
        attenuation = albedo->Value(rec.uv.x, rec.uv.y, rec.p);
        return true;
    }

public:
    std::shared_ptr<Texture> albedo;
};
