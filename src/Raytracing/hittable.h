#ifndef HITTABLE_H
#define HITTABLE_H

#include "project_utils.h"

class hit_details {
    public:
        point3 p;
        vec3 normal;
        double t;
        bool front_face;
        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};
class hittable {
    public:
        virtual ~hittable() = default;
        virtual bool hits(const ray& r, range range, hit_details& rec) const = 0;
};

#endif