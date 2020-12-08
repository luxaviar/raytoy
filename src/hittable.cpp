#include "hittable.h"
//#include "hittable_list.h"
#include "material.h"

void Hittable::FetchLight(std::vector<std::shared_ptr<Hittable>>& lights) {
    if (mat_ptr_ && mat_ptr_->IsLight()) {
        lights.push_back(shared_from_this());
    }
}

