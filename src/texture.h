#pragma once

#include <iostream>
#include <memory>
#include "perlin.h"
#include "common/image.h"

class Texture  {
public:
    virtual Color Value(XFloat u, XFloat v, const Vec3f& p) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor() {}
    SolidColor(Color c) : color_value(c) {}

    SolidColor(XFloat red, XFloat green, XFloat blue)
        : SolidColor(Color(red,green,blue)) {}

    virtual Color Value(XFloat u, XFloat v, const Vec3f& p) const override {
        return color_value;
    }

private:
    Color color_value;
};


class CheckerTexture : public Texture {
public:
    CheckerTexture() {}

    CheckerTexture(std::shared_ptr<Texture> t0, std::shared_ptr<Texture> t1)
        : even(t0), odd(t1) {}

    CheckerTexture(Color c1, Color c2)
        : even(std::make_shared<SolidColor>(c1)) , odd(std::make_shared<SolidColor>(c2)) {}

    virtual Color Value(XFloat u, XFloat v, const Vec3f& p) const override {
        auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
        if (sines < 0)
            return odd->Value(u, v, p);
        else
            return even->Value(u, v, p);
    }

public:
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;
};

class NoiseTexture : public Texture {
public:
    NoiseTexture() {}
    NoiseTexture(XFloat sc) : scale(sc) {}

    virtual Color Value(XFloat u, XFloat v, const Vec3f& p) const override {
        // return color(1,1,1)*0.5*(1 + noise.turb(scale * p));
        // return color(1,1,1)*noise.turb(scale * p);
        return Color(1,1,1)*0.5*(1 + sin(scale * p.z + 10*noise.Turb(p)));
    }

public:
    Perlin noise;
    XFloat scale;
};

class ImageTexture : public Texture {
public:
    const static int bytes_per_pixel = 3;

    ImageTexture()
        : data_(nullptr), width_(0), height_(0), bytes_per_scanline_(0) {}

    ImageTexture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data_ = image_load(
            filename, &width_, &height_, &components_per_pixel, components_per_pixel);

        if (!data_) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width_ = height_ = 0;
        }

        bytes_per_scanline_ = bytes_per_pixel * width_;
    }

    ~ImageTexture() {
        image_free(data_);
    }

    virtual Color Value(XFloat u, XFloat v, const Vec3f& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data_ == nullptr)
            return Color(0,1,1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = math::Clamp(u, 0.0, 1.0);
        v = 1.0 - math::Clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width_);
        auto j = static_cast<int>(v * height_);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width_)  i = width_-1;
        if (j >= height_) j = height_-1;

        constexpr auto color_scale = 1.0 / 255.0;
        auto pixel = data_ + j*bytes_per_scanline_ + i*bytes_per_pixel;

        return Color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

private:
    unsigned char *data_;
    int width_, height_;
    int bytes_per_scanline_;
};
