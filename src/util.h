#pragma once

#include <cmath>
#include <cstdint>
#include "math/vec3.h"

inline XFloat GammaToLinearSpaceExact (XFloat value) {
    if (value <= 0.04045f)
        return value / 12.92f;
    else if (value < 1.0f)
        return std::pow((value + 0.055f)/1.055f, 2.4f);
    else
        return std::pow(value, 2.2f);
}

inline Color GammaToLinearSpace (const Color& sRGB) {
    // Approximate version from http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
    return sRGB * (sRGB * (sRGB * 0.305306011f + 0.682171111f) + 0.012522878f);

    // Precise version, useful for debugging.
    //return Vec3f(GammaToLinearSpaceExact(sRGB.r), GammaToLinearSpaceExact(sRGB.g), GammaToLinearSpaceExact(sRGB.b));
}

inline XFloat LinearToGammaSpaceExact (XFloat value) {
    if (value <= 0.0f)
        return 0.0;
    else if (value <= 0.0031308f)
        return 12.92 * value;
    else if (value < 1.0f)
        return 1.055 * std::pow(value, 0.4166667f) - 0.055f;
    else
        return std::pow(value, 0.45454545f);
}

inline Color LinearToGammaSpace (Color linRGB) {
    linRGB = Vec3f::Max(linRGB, Vec3f::zero);
    // An almost-perfect approximation from http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
    return Vec3f::Max((XFloat)1.055f * Vec3f::Pow(linRGB, 0.416666667f) - (XFloat)0.055f, Vec3f::zero);
    
    // Exact version, useful for debugging.
    //return half3(LinearToGammaSpaceExact(linRGB.r), LinearToGammaSpaceExact(linRGB.g), LinearToGammaSpaceExact(linRGB.b))
}

inline Color ACESToneMapping(Color color, XFloat adapted_lum=1.0f) {
    constexpr XFloat A = 2.51f;
    constexpr XFloat B = 0.03f;
    constexpr XFloat C = 2.43f;
    constexpr XFloat D = 0.59f;
    constexpr XFloat E = 0.14f;

    color *= adapted_lum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

inline void CanoicalColor(Color& color) {
    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (color.r != color.r) color.r = 0.0f;
    if (color.g != color.g) color.g = 0.0f;
    if (color.b != color.b) color.b = 0.0f;
    
    // color.r = math::Clamp(color.r, 0.0, 1.0);
    // color.g = math::Clamp(color.g, 0.0, 1.0);
    // color.b = math::Clamp(color.b, 0.0, 1.0);
}

math::Vec3<uint8_t> SdrColor(Color color, XFloat inv_samples_per_pixel) {
    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    color *= inv_samples_per_pixel;

    color = LinearToGammaSpace(color);
    
    return math::Vec3<uint8_t>(256 * math::Clamp(color.r, 0.0f, 0.999f), 256 * math::Clamp(color.g, 0.0f, 0.999f), 256 * math::Clamp(color.b, 0.0f, 0.999f));
}
