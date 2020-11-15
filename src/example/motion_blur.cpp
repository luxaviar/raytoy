#include <chrono>
#include <iostream>
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "buffer.h"
#include "concurrent/thread_pool.h"
#include "common/image.h"
#include "math/vec3.h"
#include "util.h"
#include "renderer.h"

HittableList gen_scene() {
    HittableList world;

    auto ground_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.Add(std::make_shared<Sphere>(Vec3f(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = math::random::Random<XFloat>();
            Vec3f center(a + 0.9*math::random::Random<XFloat>(), 0.2, b + 0.9*math::random::Random<XFloat>());

            if ((center - Vec3f(4, 0.2, 0)).Magnitude() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = math::random::Vector() * math::random::Vector();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    Vec3f center2 = center + Vec3f(0, math::random::Random<XFloat>(0, 0.5), 0);
                    world.Add(std::make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = math::random::Vector(0.5, 1);
                    auto fuzz = math::random::Random<XFloat>(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(std::make_shared<Sphere>(Vec3f(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(Vec3f(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(std::make_shared<Sphere>(Vec3f(4, 1, 0), 1.0, material3));

    return world;
}

int main() {
    // Render
    Renderer r(100, 50, Color(0.70, 0.80, 1.00));
    
    // World
    auto world = gen_scene();
    r.BuildBVH(world);

    // Image
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 400;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    FrameBuffer image(image_width, image_height);

    // Camera
    Camera camera(
        Vec3f(13,2,3), //pos
        Vec3f(0, 0, 0), //look at pos
        Vec3f(0, 1, 0), //up vector
        20, //fov
        aspect_ratio, //aspect ratio
        0.0, //aperture
        10, //dist_to_focus
        0.0,
        1.0
    );

    r.Render(camera, image);

    write_png_image("output.png", image.width(), image.height(), 3, (const void*)image.data().data(), 0);
}
