#pragma once

#include "hittable.h"
#include <memory>
#include <vector>

class HittableList : public Hittable  {
public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { Add(object); }

    void Clear() { objects.clear(); }
    void Add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool Hit(const Ray& r, XFloat tmin, XFloat tmax, HitResult& rec) const override;

public:
    std::vector<std::shared_ptr<Hittable>> objects;
};

bool HittableList::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const {
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
