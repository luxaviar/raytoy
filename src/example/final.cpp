#include <chrono>
#include <iostream>
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "buffer.h"
#include "concurrent/thread_pool.h"
#include "common/image.h"
#include "math/vec3.h"
#include "util.h"
#include "renderer.h"
#include "texture.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "moving_sphere.h"

HittableList gen_scene() {
    HittableList world;
    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = math::random::Random(1,101);
            auto z1 = z0 + w;

            auto pos = (Vec3f(x1, y1, z1) + Vec3f(x0, y0, z0)) / 2;
            auto ext = (Vec3f(x1, y1, z1) - Vec3f(x0, y0, z0)) / 2;

            boxes1.Add(std::make_shared<Box>(pos, Quaternion::identity, ext, ground));
        }
    }

    world.Add(std::make_shared<BvhNode>(boxes1));

    auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));
    world.Add(std::make_shared<FlipFace>(std::make_shared<AARect<math::Axis::kY>>(123, 423, 147, 412, 554, light)));

    auto center1 = Vec3f(400, 400, 200);
    auto center2 = center1 + Vec3f(30,0,0);
    auto moving_sphere_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    world.Add(std::make_shared<MovingSphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    world.Add(std::make_shared<Sphere>(Vec3f(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    world.Add(std::make_shared<Sphere>(Vec3f(0, 150, 145), 50, std::make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = std::make_shared<Sphere>(Vec3f(360,150,145), 70, std::make_shared<Dielectric>(1.5));
    world.Add(boundary);
    world.Add(std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(Vec3f(0,0,0), 5000, std::make_shared<Dielectric>(1.5));
    world.Add(std::make_shared<ConstantMedium>(boundary, .0001, Color(1,1,1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("../../assets/earthmap.jpg"));
    world.Add(std::make_shared<Sphere>(Vec3f(400,200,400), 100, emat));
    auto pertext = std::make_shared<NoiseTexture>(0.1);
    world.Add(std::make_shared<Sphere>(Vec3f(220,280,300), 80, std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        auto pos = math::random::Vector(0,165);
        pos = Quaternion::AngleAxis(15, Vec3f::up) * pos;
        boxes2.Add(std::make_shared<Sphere>(pos + Vec3f(-100,270,395), 10, white));
    }

    world.Add(std::make_shared<BvhNode>(boxes2));

    return world;
}

int main() {
    // Render
    Renderer r(1000, 50, Color::zero);
    
    // World
    auto world = gen_scene();
    r.BuildBVH(world);

    // Image
    constexpr auto aspect_ratio = 1.0;
    constexpr int image_width = 800;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    FrameBuffer image(image_width, image_height);

    // Camera
    Camera camera(
        Vec3f(478, 278, -600), //pos
        Vec3f(278, 278, 0), //look at pos
        Vec3f(0, 1, 0), //up vector
        40, //fov
        aspect_ratio, //aspect ratio
        0.0, //aperture
        10, //dist_to_focus
        0,
        1
    );

    auto lights = std::make_shared<HittableList>();
    lights->Add(std::make_shared<AARect<math::Axis::kY>>(123, 423, 147, 412, 554, std::shared_ptr<Material>()));
    r.Render(camera, image, lights, 10);

    write_png_image("output.png", image.width(), image.height(), 3, (const void*)image.data().data(), 0);
}
