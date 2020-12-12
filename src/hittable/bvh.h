#pragma once

#include <algorithm>
#include "hittable.h"
#include "mesh.h"
#include "math/random.h"

class BvhNode : public Hittable  {
public:
    BvhNode(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end);

    virtual bool Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const override;
    virtual void FetchLight(std::vector<std::shared_ptr<Hittable>>& lights) override;

private:
    std::shared_ptr<Hittable> left_;
    std::shared_ptr<Hittable> right_;
};

