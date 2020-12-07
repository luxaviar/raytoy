#include "hittable.h"
#include "hittable_list.h"
#include "material.h"

void Hittable::FetchLight(std::shared_ptr<HittableList> lights) {
    if (mat_ptr_ && mat_ptr_->IsLight()) {
        lights->Add(shared_from_this());
    }
}

