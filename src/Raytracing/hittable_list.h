#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "project_utils.h"
#include <vector>
using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects;
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
        }

        //check if a ray hits anything in the list of objects
        bool hits(const ray& r, range ray_range, hit_details& hit) const override {
            double closest_t = ray_range.max;
            hit_details closest_hit;
            bool hit_anything = false;

            for (int i = 0; i < objects.size(); i++) {
                auto obj = objects[i];
                if (obj->hits(r, range(ray_range.min, closest_t), closest_hit)) {
                    hit_anything = true;
                    closest_t = closest_hit.t;
                    hit = closest_hit;
                }
            }
            return hit_anything;
        }
};

#endif