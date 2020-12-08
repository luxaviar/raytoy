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

HittableList gen_scene() {
    HittableList world;

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15,15,15));

    world.Add(std::make_shared<AARect<math::Axis::kX>>(0, 555, 0, 555, 0, green));
    world.Add(std::make_shared<AARect<math::Axis::kX>>(0, 555, 0, 555, 555, red));
    world.Add(std::make_shared<AARect<math::Axis::kY, false>>(213, 343, 227, 332, 554, light));
    world.Add(std::make_shared<AARect<math::Axis::kY>>(0, 555, 0, 555, 555, white));
    world.Add(std::make_shared<AARect<math::Axis::kY>>(0, 555, 0, 555, 0, white));
    world.Add(std::make_shared<AARect<math::Axis::kZ>>(0, 555, 0, 555, 555, white));

    std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Vec3f(192,165,295 + 82.5), Quaternion::AngleAxis(-15, Vec3f::up), Vec3f(82.5,165,82.5), aluminum);
    world.Add(box1);

    auto glass = std::make_shared<Dielectric>(1.5);
    // std::shared_ptr<Hittable> box2 = std::make_shared<Mesh>(Vec3f(367, 120, 147.5), Quaternion::identity, 110, "../../assets/suzane.obj", white);
    std::shared_ptr<Hittable> box2 = std::make_shared<Mesh>(Vec3f(367, 82.5, 147.5), Quaternion::identity, 1500, "../../assets/bunny.obj", glass, false);
    world.Add(box2);

    return world;
}

int main() {
    // Render
    Renderer r(100);
    
    // World
    auto world = gen_scene();
    r.BuildWorld(world);

    // Image
    constexpr auto aspect_ratio = 1.0; // 16.0 / 9.0;
    constexpr int image_width = 600;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);
    FrameBuffer image(image_width, image_height);

    // Camera
    Camera camera(
        Vec3f(278, 278, -800), //pos
        Vec3f(278, 278, 0), //look at pos
        Vec3f(0, 1, 0), //up vector
        40, //fov
        aspect_ratio, //aspect ratio
        0.0, //aperture
        10 //dist_to_focus
    );

    r.Render(camera, image);

    write_png_image("output.png", image.width(), image.height(), 3, (const void*)image.data().data(), 0);
}
