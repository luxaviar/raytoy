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
    XFloat PDFValue(const Vec3f& o, const Vec3f& v) const override;
    Vec3f Random(const Vec3f &o) const override;

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

XFloat HittableList::PDFValue(const Vec3f& o, const Vec3f& v) const {
    auto weight = 1.0 / objects.size();
    auto sum = 0.0;

    for (const auto& object : objects)
        sum += weight * object->PDFValue(o, v);

    return sum;
}

Vec3f HittableList::Random(const Vec3f &o) const {
    auto int_size = static_cast<int>(objects.size());
    return objects[math::random::Random(0, int_size-1)]->Random(o);
}

