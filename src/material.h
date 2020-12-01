#pragma once

#include "ray.h"
#include "hittable.h"
#include "math/random.h"
#include "texture.h"
#include "pdf.h"

struct ScatterRecord {
    Ray specular_ray;
    bool is_specular;
    Color attenuation;
    std::shared_ptr<PDF> pdf_ptr;
};

class Material {
public:
    virtual bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const = 0;
    
    virtual double ScatteringPDF(const Ray& r_in, const HitResult& rec, const Ray& scattered) const {
        return 0;
    }

    virtual Color Emitted(const Ray& r_in, const HitResult& rec, XFloat u, XFloat v, const Vec3f& p) const {
        return Color::zero;
    }
};

class Lambertian : public Material {
public:
    Lambertian(const Color& a) : albedo(std::make_shared<SolidColor>(a)) {}
    Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

    bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const override {
        srec.is_specular = false;
        srec.attenuation = albedo->Value(rec.uv.u, rec.uv.v, rec.p);
        srec.pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
        return true;
    }

    XFloat ScatteringPDF(const Ray& r_in, const HitResult& rec, const Ray& scattered) const override {
        auto cosine = rec.normal.Dot(scattered.direction.Normalize());
        return cosine < 0 ? 0 : cosine / math::kPI;
    }

public:
    std::shared_ptr<Texture> albedo;
};

class Metal : public Material {
public:
    Metal(const Color& a, XFloat f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const override {
        Vec3f reflected = Vec3f::Reflect(r_in.direction.Normalize(), rec.normal);
        srec.specular_ray = Ray(rec.p, reflected + fuzz * math::random::Vector(), r_in.time);
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        return true;
    }

public:
    Color albedo;
    XFloat fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(XFloat index_of_refraction) : ir(index_of_refraction) {}

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const override {
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = Color(1.0, 1.0, 1.0);
        XFloat refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        Vec3f unit_direction = r_in.direction.Normalize();
        XFloat cos_theta = fmin(rec.normal.Dot(-unit_direction), 1.0);
        XFloat sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vec3f direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > math::random::Random<XFloat>())
            direction = Vec3f::Reflect(unit_direction, rec.normal);
        else
            direction = Vec3f::Refract(unit_direction, rec.normal, refraction_ratio);

        srec.specular_ray = Ray(rec.p, direction, r_in.time);
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

    virtual bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const override {
        return false;
    }

    virtual Color Emitted(const Ray& r_in, const HitResult& rec, XFloat u, XFloat v, const Vec3f& p) const override {
        if (!rec.front_face)
            return Color(0,0,0);
        return emit->Value(u, v, p);
    }

public:
    std::shared_ptr<Texture> emit;
};

class Isotropic : public Material {
public:
    Isotropic(Color c) : albedo(std::make_shared<SolidColor>(c)) {}
    Isotropic(std::shared_ptr<Texture> a) : albedo(a) {}

    // virtual bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const override {
    //     scattered = Ray(rec.p, math::random::UnitVector(), r_in.time);
    //     attenuation = albedo->Value(rec.uv.x, rec.uv.y, rec.p);
    //     return true;
    // }
    virtual bool Scatter(const Ray& r_in, const HitResult& rec, ScatterRecord& srec) const override {
        srec.is_specular = false;
        srec.attenuation = albedo->Value(rec.uv.u, rec.uv.v, rec.p);
        srec.pdf_ptr = std::make_shared<SpherePDF>(rec.p);
        return true;
    }

    virtual double ScatteringPDF(const Ray& r_in, const HitResult& rec, const Ray& scattered) const {
        return 1.0 / (4.0 * math::kPI);
    }

public:
    std::shared_ptr<Texture> albedo;
};

