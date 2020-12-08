#pragma once

#include "hittable.h"
#include <memory>
#include <vector>
#include "math/random.h"
#include "bvh.h"

class HittableList : public Hittable  {
public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { Add(object); }

    size_t size() const { return objects.size(); }
    bool empty() const { return objects.size() == 0; }

    void Clear() { objects.clear(); }
    void Add(std::shared_ptr<Hittable> object) {
        objects.push_back(object); 
        if (objects.size() == 1) {
            bounding_box_ = object->bounding_box();
        } else {
            bounding_box_ = AABB::Union(bounding_box_, object->bounding_box());
        }
    }

    void BuildBVH() override {
        if (objects.empty()) return;

        root = std::make_shared<BvhNode>(objects, 0, objects.size());
    }

    bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const override {
        if (!root) return false;
        return root->Hit(r, t_min, t_max, rec);
    }

    XFloat PDF(const Vec3f& o, const Vec3f& wo) const override {
        if (empty()) return 0;

        auto weight = 1.0 / objects.size();
        auto sum = 0.0;

        for (const auto& object : objects)
            sum += object->PDF(o, wo);

        return sum * weight;
    }

    Vec3f Sample(const Vec3f &o) const override {
        if (empty()) {
            return Vec3f::zero;
        }
        
        auto int_size = static_cast<int>(objects.size());
        return objects[math::random::Random(0, int_size-1)]->Sample(o);
    }

    void FetchLight(std::vector<std::shared_ptr<Hittable>>& lights) override {
        for (auto& object : objects)
            object->FetchLight(lights);
    }

public:
    std::vector<std::shared_ptr<Hittable>> objects;
    std::shared_ptr<BvhNode> root;
};

