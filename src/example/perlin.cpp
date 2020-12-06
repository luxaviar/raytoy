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

HittableList gen_scene() {
    HittableList world;

    auto pertext = std::make_shared<NoiseTexture>(4);
    world.Add(std::make_shared<Sphere>(Vec3f(0,-1000,0), 1000, std::make_shared<Lambertian>(pertext)));
    world.Add(std::make_shared<Sphere>(Vec3f(0,2,0), 2, std::make_shared<Lambertian>(pertext)));
    
    return world;
}

int main() {
    // Render
    Renderer r(10, 0.8, Color(0.70, 0.80, 1.00));
    
    // World
    auto world = gen_scene();
    r.BuildBVH(world);

    // Image
    constexpr auto aspect_ratio = 16.0 / 9.0;
    constexpr int image_width = 1200;
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
        10 //dist_to_focus
    );

    r.Render(camera, image);

    write_png_image("output.png", image.width(), image.height(), 3, (const void*)image.data().data(), 0);
}
