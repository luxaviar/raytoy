#pragma once
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/util.h"
#include "math/random.h"
#include "common/ray.h"

class Camera {
public:
    Camera() : Camera(Vec3f(0,0,-1), Vec3f(0,0,0), Vec3f(0,1,0), 40, 1, 0, 10) {}

    Camera(
        Vec3f lookfrom,
        Vec3f lookat,
        Vec3f vup,
        XFloat vfov, // vertical field-of-view in degrees
        XFloat aspect_ratio,
        XFloat aperture,
        XFloat focus_dist,
        XFloat t0 = 0,
        XFloat t1 = 0
    ) {
        auto theta = vfov * math::kDeg2Rad;
        auto h = tan(theta/2);
        auto viewport_height = 2.0f * h;
        auto viewport_width = aspect_ratio * viewport_height;

        //left-hand
        w = (lookat -  lookfrom).Normalize(); //to lookat
        u = vup.Cross(w).Normalize(); //to right
        v = w.Cross(u); //to up

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin + focus_dist*w - horizontal/2 - vertical/2;

        lens_radius = aperture / 2;
        time0 = t0;
        time1 = t1;
    }

    Ray CastRay(XFloat s, XFloat t) const {
        Vec2f rd = lens_radius * math::random::PointInsideUnitCircle();
        Vec3f offset = u * rd.x + v * rd.y;
        return Ray(
            origin + offset,
            (lower_left_corner + s*horizontal + t*vertical - origin - offset).Normalize(),
            math::random::Random<XFloat>(time0, time1)
        );
    }

private:
    Vec3f origin;
    Vec3f lower_left_corner;
    Vec3f horizontal;
    Vec3f vertical;
    Vec3f u, v, w;
    XFloat lens_radius;
    XFloat time0, time1;  // shutter open/close times
};
