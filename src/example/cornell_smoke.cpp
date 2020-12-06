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

HittableList gen_scene() {
    HittableList world;

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(7,7,7));

    world.Add(std::make_shared<AARect<math::Axis::kX>>(0, 555, 0, 555, 0, green));
    world.Add(std::make_shared<AARect<math::Axis::kX>>(0, 555, 0, 555, 555, red));
    //world.Add(std::make_shared<AARect<math::Axis::kY>>(113, 443, 127, 432, 554, light));
    world.Add(std::make_shared<FlipFace>(std::make_shared<AARect<math::Axis::kY>>(113, 443, 127, 432, 554, light)));
    world.Add(std::make_shared<AARect<math::Axis::kY>>(0, 555, 0, 555, 555, white));
    world.Add(std::make_shared<AARect<math::Axis::kY>>(0, 555, 0, 555, 0, white));
    world.Add(std::make_shared<AARect<math::Axis::kZ>>(0, 555, 0, 555, 555, white));

    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Vec3f(192,165,295 + 82.5), Quaternion::AngleAxis(-15, Vec3f::up), Vec3f(82.5,165,82.5), white);
    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Vec3f(367,0 + 82.5,65 + 82.5), Quaternion::AngleAxis(18, Vec3f::up), Vec3f(82.5,82.5,82.5), white);
    
    world.Add(std::make_shared<ConstantMedium>(box1, 0.01, Color(0,0,0)));
    world.Add(std::make_shared<ConstantMedium>(box2, 0.01, Color(1,1,1)));

    return world;
}

int main() {
    // Render
    Renderer r(1000);
    
    // World
    auto world = gen_scene();
    r.BuildBVH(world);

    // Image
    constexpr auto aspect_ratio = 16.0 / 9.0;
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

    auto lights = std::make_shared<HittableList>();
    lights->Add(std::make_shared<AARect<math::Axis::kY>>(113, 443, 127, 432, 554, std::shared_ptr<Material>()));
    r.Render(camera, image, lights);

    write_png_image("output.png", image.width(), image.height(), 3, (const void*)image.data().data(), 0);
}
