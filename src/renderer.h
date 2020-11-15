#pragma once

#include <chrono>
#include <iostream>
#include <cstdint>
#include "common/uncopyable.h"
#include "math/vec3.h"
#include "buffer.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "util.h"
#include "concurrent/thread_pool.h"
#include "bvh.h"

using FrameBuffer = Buffer<math::Vec3<uint8_t>>;

struct TraceSpec {
    int width;
    int height;
    int samples_per_pixel;
    int max_depth;
    XFloat inv_width;
    XFloat inv_height;
    XFloat inv_samples_per_pixel;

    FrameBuffer* image;
    const Camera* camera;
};

class Renderer : private Uncopyable {
public:
    Renderer(int samples_per_pixel, int max_depth, Color background_color = Color::zero) : background_color_(background_color) {
        spec_.samples_per_pixel = samples_per_pixel;
        spec_.inv_samples_per_pixel = 1.0 / samples_per_pixel;
        spec_.max_depth = max_depth;
    }

    TraceSpec& spec() { return spec_; }
    void BuildBVH(const HittableList& world);

    void Render(const Camera& camera, FrameBuffer& image, int parallel = 8);

private:
    void CastRay(int begin_line, int end_line);
    Color Trace(const Ray& r, int depth);

    Color background_color_;
    TraceSpec spec_;
    BvhNode root_;
};

void Renderer::BuildBVH(const HittableList& world) {
    root_ = BvhNode(world);
}

Color Renderer::Trace(const Ray& r, int depth) {
    HitResult rec;

    if (depth <= 0)
        return Color(0, 0, 0);

    if (!root_.Hit(r, 0.001, math::kInfinite, rec)) {
        return background_color_;
    }

    Ray scattered;
    Color attenuation;
    Color emitted = rec.mat_ptr->Emitted(rec.uv.u, rec.uv.v, rec.p);

    if (!rec.mat_ptr->Scatter(r, rec, attenuation, scattered))
        return emitted;
    
    return emitted + attenuation * Trace(scattered, depth-1);
}

void Renderer::CastRay(int begin_line, int end_line) {
    auto begin = std::chrono::steady_clock::now();
    for (int j = begin_line; j < end_line; ++j) {
        for (int i = 0; i < spec_.width; ++i) {
            Color pixel_color(0,0,0);
            for (int s = 0; s < spec_.samples_per_pixel; ++s) {
                auto u = (i + math::random::Random<XFloat>()) * spec_.inv_width;
                auto v = (j + math::random::Random<XFloat>()) * spec_.inv_height;
                Ray r = spec_.camera->CastRay(u, v);
                pixel_color += Trace(r, spec_.max_depth);
            }

            spec_.image->Set(i, (spec_.height - 1) - j, SdrColor(pixel_color, spec_.inv_samples_per_pixel));
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - begin;

    std::cerr << "finish " << begin_line << ", " << end_line << " in " << diff.count() << std::endl;
}

void Renderer::Render(const Camera& camera, FrameBuffer& image, int parallel) {
    auto begin = std::chrono::steady_clock::now();

    int width = image.width();
    int height = image.height();

    spec_.width = width;
    spec_.height = height;
    spec_.inv_width = 1.0f / (width - 1);
    spec_.inv_height = 1.0f / (height - 1);
    spec_.image = &image;
    spec_.camera = &camera;

    //int parallel = 8;
    int task_span = height / parallel;
    ThreadPool pool(parallel);
    for (int i = 0; i < parallel - 1; ++i) {
        pool.Enqueue([=] {
            CastRay(i * task_span, i * task_span + task_span);
        });
    }
    pool.Enqueue([=] {
        CastRay(task_span * (parallel - 1), height);
    });

    pool.Join();
    
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - begin;
    std::cerr << "finish in " << diff.count() << std::endl;
}
