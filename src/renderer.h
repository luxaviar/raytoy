#pragma once

#include <chrono>
#include <iostream>
#include <cstdint>
#include "common/uncopyable.h"
#include "math/vec3.h"
#include "common/buffer.h"
#include "camera.h"
#include "hittable/hittable.h"
#include "hittable/hittable_list.h"
#include "hittable/bvh.h"
#include "util.h"
#include "concurrent/thread_pool.h"

using FrameBuffer = Buffer<math::Vec3<uint8_t>>;

struct TraceSpec {
    int width;
    int height;
    int samples_per_pixel;
    // XFloat rr; //RussianRoulette
    int depth;
    XFloat inv_width;
    XFloat inv_height;
    XFloat inv_samples_per_pixel;

    FrameBuffer* image;
    const Camera* camera;
};

struct RaySpan {
    RaySpan(int b, int e) : begin(b), end(e) {}

    int begin;
    int end;
};

class Renderer : private Uncopyable {
public:
    Renderer(int samples_per_pixel, Color background_color = Color::zero, int depth = 50) : 
        background_color_(background_color), 
        lights_(std::make_shared<HittableList>()) 
    {
        spec_.samples_per_pixel = samples_per_pixel;
        spec_.inv_samples_per_pixel = 1.0 / samples_per_pixel;
        spec_.depth = depth;
    }

    TraceSpec& spec() { return spec_; }
    void BuildWorld(const HittableList& world);
    std::shared_ptr<HittableList> lights() { return lights_; }

    void Render(const Camera& camera, FrameBuffer& image, int parallel = 8, int span=256);

private:
    void CastRay(int begin, int end);
    Color Trace(const Ray& r, int depth);

    Color background_color_;
    TraceSpec spec_;
    std::shared_ptr<BvhNode> root_;
    std::mutex mutex_;
    std::shared_ptr<HittableList> lights_;
};

void Renderer::BuildWorld(const HittableList& world) {
    root_ = std::make_shared<BvhNode>(world.objects, 0, world.objects.size());

    std::vector<std::shared_ptr<Hittable>> lights;
    root_->FetchLight(lights);

    for (auto l : lights) {
        lights_->Add(l);
    }
}

Color Renderer::Trace(const Ray& r, int depth) {
    HitResult res;

    if (depth < 0)
        return Color(0, 0, 0);

    if (!root_->Hit(r, 0.001, math::kInfinite, res)) {
        return background_color_;
    }

    ScatterRecord srec;
    Color emitted = res.mat_ptr->Emitted(r, res, res.uv.u, res.uv.v, res.p);

    if (!res.mat_ptr->Scatter(r, res, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation * Trace(srec.specular_ray, depth - 1);
    }

    if (lights_) {
        HittablePDF light_ptr(lights_);
        MixturePDF mp(&light_ptr, srec.pdf_ptr);
        XFloat pdf_val;
        auto wo = mp.Sample(res, pdf_val);
        Ray scattered(res.p, wo, r.time);

        return emitted +
            srec.attenuation * res.mat_ptr->ScatteringPDF(r, res, scattered) * Trace(scattered, depth - 1) / pdf_val;
    } else {
        XFloat pdf_val;
        auto wo = srec.pdf_ptr->Sample(res, pdf_val);
        Ray scattered(res.p, wo, r.time);

        return emitted + 
            srec.attenuation * res.mat_ptr->ScatteringPDF(r, res, scattered) * Trace(scattered, depth - 1) / pdf_val;
    }
}

void Renderer::CastRay(int begin, int end) {
    for (int x = begin; x <= end; ++x) {
        int j = x / spec_.width;
        int i = x - j * spec_.width;

        Color pixel_color(0,0,0);
        for (int s = 0; s < spec_.samples_per_pixel; ++s) {
            auto u = (i + math::random::Random<XFloat>()) * spec_.inv_width;
            auto v = (j + math::random::Random<XFloat>()) * spec_.inv_height;
            Ray r = spec_.camera->CastRay(u, v);
            
            Color color = Trace(r, spec_.depth);
            CanoicalColor(color);

            pixel_color += color;
        }

        spec_.image->Set(i, (spec_.height - 1) - j, SdrColor(pixel_color, spec_.inv_samples_per_pixel));
    }
}

void Renderer::Render(const Camera& camera, FrameBuffer& image, int parallel, int span) {
    auto begin = std::chrono::steady_clock::now();

    int width = image.width();
    int height = image.height();

    spec_.width = width;
    spec_.height = height;
    spec_.inv_width = 1.0f / (width - 1);
    spec_.inv_height = 1.0f / (height - 1);
    spec_.image = &image;
    spec_.camera = &camera;

    int total = width * height;
    int count = total / span;
    std::vector<RaySpan> jobs;
    jobs.reserve(count + 1);
    for (int i = 0; i < count;  ++i) {
        jobs.emplace_back(i * span, i * span + span - 1);
    }

    if (count * span < total) {
        jobs.emplace_back(count * span, total - 1); 
    }

    std::cout << "Total number of jobs: " << count << std::endl;

    ThreadPool pool(parallel);
    for (int i = 0; i < parallel; ++i) {
        pool.Enqueue([=, &jobs] {
            while(true) {
                int begin = -1;
                int end = -1;
                {
                    std::lock_guard<std::mutex> lk(mutex_);
                    if (jobs.empty()) {
                        return;
                    }
                    auto rs = jobs.back();
                    begin = rs.begin;
                    end = rs.end;
                    jobs.pop_back();
                    std::cerr << jobs.size() << "      \r";
                }
                CastRay(begin, end);
            }
        });
    }

    pool.Join();
    
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - begin;
    std::cerr << "Finish in " << diff.count() << "s" << std::endl;
}
 
