#pragma once

#include <cmath>
#include <limits>

#define XFloat double

namespace math {
    //constexpr float kEpsilon = std::numeric_limits<float>::epsilon();
    constexpr XFloat kEpsilon = 0.00001;
    constexpr XFloat kEpsilonSq = kEpsilon * kEpsilon;
    constexpr XFloat kPI = 3.14159265358979323;
    constexpr XFloat kInvPI = 1.0 / kPI;
    constexpr XFloat kDeg2Rad = kPI / 180.0;
    constexpr XFloat kRad2Deg = 180.0 / kPI;
    constexpr XFloat kInfinite = std::numeric_limits<XFloat>::infinity();

    template<typename T>
    T Sign(T v) {
        if (v >= 0) return 1;
        return -1;
    }

    template<typename T>
    T Abs(T v) {
        if (v < 0) v = -v;
        return v;
    }

    template<typename T>
    T Max(T a, T b) {
        return a > b ? a : b;
    }

    template<typename T>
    T Min(T a, T b) {
        return a > b ? b : a;
    }

    template<typename T>
    T Pow(T a, T b) {
        return ::pow(a, b);
    }

    inline float Lerp(float a, float b, float t) {
        return a * (1.0f - t) + b * t;
    }

    inline double Lerp(double a, double b, double t) {
        return a * (1.0 - t) + b * t;
    }
    
    template<typename T>
    T Clamp(T v, T min, T max) {
        if (v < min) return min;
        else if (v > max) return max;
        return v;
    }

    inline float Clamp(float v, float min, float max) {
        return Clamp<float>(v, min, max);
    }

    inline float Saturate(float v) {
        return Clamp<float>(v, 0.0f, 1.0f);
    }

    inline float Saturate(double v) {
        return Clamp<double>(v, 0.0, 1.0);
    }

    inline bool AlmostEqual(float left, float right, float epsilon = kEpsilon) {
        return Abs(left - right) < epsilon;
    }

    inline bool AlmostEqual(double left, double right, double epsilon = kEpsilon) {
        return Abs(left - right) < epsilon;
    }
}


