#pragma once

#include <algorithm>
#include "util.h"
#include "vec4.h"
#include "quat.h"

namespace math {

template<typename T>
struct Mat4x4 {
    using value_type = T;
    static constexpr int rows = 4;
    static constexpr int cols = 4;

    static const Mat4x4<T> identity;
    static const Mat4x4<T> zero;

    union {
        T m[rows][cols];
        struct { Vec4<T> r[rows]; };
        struct { Vec4<T> r0, r1, r2, r3; };
    };

    constexpr Mat4x4() {}

    constexpr Mat4x4(T m00, T m01, T m02, T m03,
                      T m10, T m11, T m12, T m13,
                      T m20, T m21, T m22, T m23,
                      T m30, T m31, T m32, T m33)
        : m{m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33} {
    }

    constexpr Mat4x4(const Vec4<T>& r0_, const Vec4<T>& r1_, const Vec4<T>& r2_, const Vec4<T>& r3_)
        : r0(r0_), r1(r1_), r2(r2_), r3(r3_) {
    }

    constexpr Mat4x4(const Mat4x4<T>& m) 
        : r0(m.r0), r1(m.r1), r2(m.r2), r3(m.r3) {
    }

    void Zero() {
        r0.Zero();
        r1.Zero();
        r2.Zero();
        r3.Zero();
    }

    Mat4x4<T> Transpose() const {
        return Mat4x4<T>(
            r0.x, r1.x, r2.x, r3.x,
            r0.y, r1.y, r2.y, r3.y,
            r0.z, r1.z, r2.z, r3.z,
            r0.w, r1.w, r2.w, r3.w
        );
    }

    //scale rotation
    Vec3<T> MultiplyVector(const Vec3<T>& v) const {
        return Vec3<T>(
            (r0.x * v.x + r0.y * v.y + r0.z * v.z),
            (r1.x * v.x + r1.y * v.y + r1.z * v.z),
            (r2.x * v.x + r2.y * v.y + r2.z * v.z)
        );
    }

    //scale rotation
    Vec4<T> MultiplyVector(const Vec4<T>& v) const {
        return Vec4<T>(
            (r0.x * v.x + r0.y * v.y + r0.z * v.z),
            (r1.x * v.x + r1.y * v.y + r1.z * v.z),
            (r2.x * v.x + r2.y * v.y + r2.z * v.z),
            v.w
            );
    }

    //scale rotation translate
    Vec3<T> MultiplyPoint3X4(const Vec3<T>& v) const {
        return Vec3<T>(
            (r0.x * v.x + r0.y * v.y + r0.z * v.z + r0.w),
            (r1.x * v.x + r1.y * v.y + r1.z * v.z + r1.w),
            (r2.x * v.x + r2.y * v.y + r2.z * v.z + r2.w)
        );
    }

    //scale rotation translate projection
    Vec3<T> MultiplyPoint(const Vec3<T>& v) const {
        Vec3<T> res(
            (r0.x * v.x + r0.y * v.y + r0.z * v.z + r0.w),
            (r1.x * v.x + r1.y * v.y + r1.z * v.z + r1.w),
            (r2.x * v.x + r2.y * v.y + r2.z * v.z + r2.w)
        );
        T w = (r3.x * v.x + r3.y * v.y + r3.z * v.z + r3.w);
        if (Abs(w) > 1.0e-7f) {
            T invW = 1.0f / w;
            res.x *= invW;
            res.y *= invW;
            res.z *= invW;
        } else {
            res.x = 0;
            res.y = 0;
            res.z = 0;
        }

        return res;
    }

    T Determinant() const {
        return (r0.x * (r1.y * r2.z - r2.y * r1.z) -
            r0.y * (r1.x * r2.z - r2.x * r1.z) +
            r0.z * (r1.x * r2.y - r2.x * r1.y));
    }

    Mat4x4<T>& InverseOrthonormal() {
        std::swap(r0.y, r1.x);
        std::swap(r0.z, r2.x);
        std::swap(r1.z, r2.y);

        Vec3<T> t(-r0.w, -r1.w, -r2.w);
        Vec3<T> trans = MultiplyVector(t);
        r0.w = trans.x;
        r1.w = trans.y;
        r2.w = trans.z;

        return *this;
    }

    T operator()(int row, int col) const {
        return m[row][col];
    }

    T& operator()(int row, int col) {
        return m[row][col];
    }

    Mat4x4<T>& operator =(const Mat4x4<T>& v) {
        r0 = v.r0;
        r1 = v.r1;
        r2 = v.r2;
        r3 = v.r3;

        return *this;
    }

    Mat4x4<T>& operator *=(T v) {
        r0 *= v;
        r1 *= v;
        r2 *= v;
        r3 *= v;
        return *this;
    }

    Mat4x4<T>& operator -=(const Mat4x4<T>& v) {
        r0 -= v.r0;
        r1 -= v.r1;
        r2 -= v.r2;
        r3 -= v.r3;
        return *this;
    }

    Mat4x4<T>& operator +=(const Mat4x4<T>& v) {
        r0 += v.r0;
        r1 += v.r1;
        r2 += v.r2;
        r3 += v.r3;
        return *this;
    }
    
    Mat4x4<T> operator +(const Mat4x4<T>& v) {
        return Mat4x4<T>(
            r0 + v.r0,
            r1 + v.r1,
            r2 + v.r2,
            r3 + v.r3,
        )
    }

    Mat4x4<T>& operator *=(const Mat4x4<T>& v) {
        r0 = v.r0 * r0.x + v.r1 * r0.y + v.r2 * r0.z + v.r3 * r0.w;
        r1 = v.r0 * r1.x + v.r1 * r1.y + v.r2 * r1.z + v.r3 * r1.w;
        r2 = v.r0 * r2.x + v.r1 * r2.y + v.r2 * r2.z + v.r3 * r2.w;
        r3 = v.r0 * r3.x + v.r1 * r3.y + v.r2 * r3.z + v.r3 * r3.w;
        return *this;
    }

    Mat4x4<T> operator *(const Mat4x4<T>& v) const {
        return Mat4x4<T>(
            v.r0 * r0.x + v.r1 * r0.y + v.r2 * r0.z + v.r3 * r0.w,
            v.r0 * r1.x + v.r1 * r1.y + v.r2 * r1.z + v.r3 * r1.w,
            v.r0 * r2.x + v.r1 * r2.y + v.r2 * r2.z + v.r3 * r2.w,
            v.r0 * r3.x + v.r1 * r3.y + v.r2 * r3.z + v.r3 * r3.w
        );
    }
    
    Vec4<T> operator *(const Vec4<T>& v) const {
        return Vec4<T>(
            v.x * r0.x + v.y * r0.y + v.z * r0.z + v.w * r0.w,
            v.x * r1.x + v.y * r1.y + v.z * r1.z + v.w * r1.w,
            v.x * r2.x + v.y * r2.y + v.z * r2.z + v.w * r2.w,
            v.x * r3.x + v.y * r3.y + v.z * r3.z + v.w * r3.w
        );
    }

    Mat4x4<T> operator *(T v) const {
        return Mat4xr<T>(
            r0 * v,
            r1 * v,
            r2 * v,
            r3 * v
        );
    }

    static Mat4x4<T> TRS(const Vec3<T>& pos, const Quat<T>& rot, const Vec3<T>& scale) {
        float x = rot.x + rot.x;
        float y = rot.y + rot.y;
        float z = rot.z + rot.z;
        float xx = rot.x * x;
        float yy = rot.y * y;
        float zz = rot.z * z;
        float xy = rot.x * y;
        float xz = rot.x * z;
        float yz = rot.y * z;
        float wx = rot.w * x;
        float wy = rot.w * y;
        float wz = rot.w * z;

        return Mat4x4<T>(
            (1.0f - (yy + zz)) * scale.x, (xy - wz) * scale.y, (xz + wy) * scale.z, pos.x,
            (xy + wz) * scale.x, (1.0f - (xx + zz)) * scale.y, (yz - wx) * scale.z, pos.y,
            (xz - wy) * scale.x, (yz + wx) * scale.y, (1.0f - (xx + yy)) * scale.z, pos.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static Mat4x4<T> TR(const Vec3<T>& pos, const Mat3x3<T>& rot) {
        return Mat4x4<T>(
            rot.r0.x, rot.r0.y, rot.r0.z, pos.x,
            rot.r1.x, rot.r1.y, rot.r1.z, pos.y,
            rot.r2.x, rot.r2.y, rot.r2.z, pos.z,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    static Mat4x4<T> TR(const Vec3<T>& pos, const Quat<T>& rot) {
        return TR(pos, rot.ToMatrix());
    }

    static Mat4x4<T> LookAt(const Vec3<T>& pos, const Vec3<T>& target, const Vec3<T>& up) {
        auto lookat = (target - pos).Normalize(); //g
        auto right = up.Cross(lookat).Normalize(); //r = t x g = -g x t
        auto nup = lookat.Cross(right).Normalize(); //u = g x r = r x -g

        return Matrix4x4(
            right.x,right.y,right.z,-pos.Dot(right),
            nup.x,nup.y,nup.z,-pos.Dot(nup),
            lookat.x,lookat.y,lookat.z,-pos.Dot(lookat),
            0,0,0,1
        );
    }
};

template<typename T>
const Mat4x4<T> Mat4x4<T>::identity(
    (T)1, (T)0, (T)0, (T)0,
    (T)0, (T)1, (T)0, (T)0,
    (T)0, (T)0, (T)1, (T)0,
    (T)0, (T)0, (T)0, (T)1
);

template<typename T>
const Mat4x4<T> Mat4x4<T>::zero(
    (T)0, (T)0, (T)0, (T)0,
    (T)0, (T)0, (T)0, (T)0,
    (T)0, (T)0, (T)0, (T)0,
    (T)0, (T)0, (T)0, (T)0
);

}

using Matrix4x4 = math::Mat4x4<XFloat>;
