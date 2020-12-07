#pragma once

#include "hittable.h"
#include <memory>
#include <vector>
#include "math/random.h"

class HittableList : public Hittable  {
public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { Add(object); }

    size_t size() const { return objects.size(); }
    bool empty() const { return objects.size() == 0; }

    void Clear() { objects.clear(); }
    void Add(std::shared_ptr<Hittable> object) { objects.push_back(object); }
        
    bool HittableList::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const override {
        HitResult temp_rec;
        auto hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto& object : objects) {
            if (object->Hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    XFloat HittableList::PDF(const Vec3f& o, const Vec3f& v) const override {
        if (empty()) return 0;

        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto& object : objects)
            sum += object->PDF(o, v);

        return sum * weight;
    }

    Vec3f HittableList::Sample(const Vec3f &o) const override {
        if (empty()) return Vec3f::zero;
        
        auto int_size = static_cast<int>(objects.size());
        return objects[math::random::Random(0, int_size-1)]->Sample(o);
    }

    void HittableList::FetchLight(std::shared_ptr<HittableList> lights) override {
        for (auto& object : objects)
            object->FetchLight(lights);
    }

public:
    std::vector<std::shared_ptr<Hittable>> objects;
};
