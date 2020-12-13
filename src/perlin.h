#pragma once

#include "math/vec3.h"
#include "math/random.h"
#include <vector>

class Perlin {
public:
    Perlin() {
        random_vector_.reserve(point_count);
        for (int i = 0; i < point_count; ++i) {
            random_vector_.push_back(math::random::Vector(-1.0, 1.0).Normalize());
        }

        GeneratePerm(perm_x_, point_count);
        GeneratePerm(perm_y_, point_count);
        GeneratePerm(perm_z_, point_count);
    }

    XFloat Noise(const Vec3f& p) const {
        auto ix = floor(p.x);
        auto iy = floor(p.y);
        auto iz = floor(p.z);

        //find the lattice
        auto i = static_cast<int>(ix);
        auto j = static_cast<int>(iy);
        auto k = static_cast<int>(iz);

        Vec3f c[2][2][2];
        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[di][dj][dk] = random_vector_[
                        perm_x_[(i+di) & 255] ^
                        perm_y_[(j+dj) & 255] ^
                        perm_z_[(k+dk) & 255]
                    ];

        //interpolate
        return Interp(c, Vec3f(p.x - ix, p.y - iy, p.z - iz));
    }

    XFloat Turb(const Vec3f& p, int depth=7) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        //fbm(Fractal Brownian Motion)
        for (int i = 0; i < depth; i++) {
            accum += weight * Noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        //turb
        return fabs(accum);

    }

private:
    static constexpr int point_count = 256;
    std::vector<Vec3f> random_vector_;
    std::vector<int> perm_x_;
    std::vector<int> perm_y_;
    std::vector<int> perm_z_;

    void GeneratePerm(std::vector<int>& p, int n) {
        p.reserve(n);
        for (int i = 0; i < n; i++) {
            p.push_back(i);
        }

        for (int i = n-1; i > 0; i--) {
            int target = math::random::Random(0,i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static XFloat Grad(Vec3f (&c)[2][2][2], const Vec3f& p, int i, int j, int k) {
        return c[i][j][k].Dot(Vec3f(p.x - i, p.y - j, p.z - k));
    }

    static XFloat Interp(Vec3f (&c)[2][2][2], const Vec3f& p) {
        // auto w = p * p * p * (p * (p * 6 - 15) + 10);

        // XFloat x1, x2, y1, y2;
        // x1 = math::Lerp(Grad(c, p, 0, 0, 0), Grad(c, p, 1, 0, 0), w.x);
        // x2 = math::Lerp(Grad(c, p, 0, 1, 0), Grad(c, p, 1, 1, 0), w.x);
        // y1 = math::Lerp(x1, x2, w.y);

        // x1 = math::Lerp(Grad(c, p, 0, 0, 1), Grad(c, p, 1, 0, 1), w.x);
        // x2 = math::Lerp(Grad(c, p, 0, 1, 1), Grad(c, p, 1, 1, 1), w.x);
        // y2 = math::Lerp(x1, x2, w.y);

        // return math::Lerp(y1, y2, w.z);

        auto w = p * p * ((XFloat)3.0 - (XFloat)2.0 * p);
        auto accum = 0.0;

        for (int i=0; i < 2; i++)
            for (int j=0; j < 2; j++)
                for (int k=0; k < 2; k++) {
                    Vec3f weight_v(p.x-i, p.y-j, p.z-k);
                    accum += (i*w.x + (1-i)*(1-w.x))*
                        (j*w.y + (1-j)*(1-w.y))*
                        (k*w.z + (1-k)*(1-w.z))*(c[i][j][k].Dot(weight_v));
                }

        return accum;
    }
};
