#include "bvh.h"

BvhNode::BvhNode(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end) {
    std::vector<std::shared_ptr<Hittable>> objects = src_objects; // Create a modifiable array of the source scene objects
    size_t object_span = end - start;
    int axis = math::random::Random(0, 2);

    if (object_span == 1) {
        left_ = objects[start];
        left_->BuildBVH();
    } else if (object_span == 2) {
        if (objects[start]->bounding_box().Compare(objects[start+1]->bounding_box(), axis)) {
            left_ = objects[start];
            right_ = objects[start+1];
        } else {
            left_ = objects[start+1];
            right_ = objects[start];
        }
        left_->BuildBVH();
        right_->BuildBVH();
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, [axis](const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b){
            return a->bounding_box().Compare(b->bounding_box(), axis);
        });

        auto mid = start + object_span / 2;
        left_ = std::make_shared<BvhNode>(objects, start, mid);
        right_ = std::make_shared<BvhNode>(objects, mid, end);
    }

    if (right_) {
        bounding_box_ = AABB::Union(left_->bounding_box(), right_->bounding_box());
    } else {
        bounding_box_ = left_->bounding_box();
    }
}

bool BvhNode::Hit(const Ray& r, XFloat t_min, XFloat t_max, HitResult& rec) const {
    if (!bounding_box_.Hit(r, t_min, t_max))
        return false;

    bool hit_left = left_->Hit(r, t_min, t_max, rec);
    bool hit_right = right_ ? right_->Hit(r, t_min, hit_left ? rec.t : t_max, rec) : false;

    return hit_left || hit_right;
}

void BvhNode::FetchLight(std::vector<std::shared_ptr<Hittable>>& lights) {
    left_->FetchLight(lights);
    if (right_) {
        right_->FetchLight(lights);
    }
}

