#ifndef SPHERE_H
#define SPHERE_H
#include "hittable.h"
#include "project_utils.h"

class sphere : public hittable {
    public:
        sphere(const point3& center, double radius) : center(center), radius(fmax(0,radius)) {}
        
        /**if the ray hits this sphere, set rec with this hit and return true */
        bool hits(const ray& r, range range, hit_details& rec) const override {
            //solve quadratic equation to find where the ray intersects with the sphere
            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;
            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;
            auto sqrtd = sqrt(discriminant);
            auto t = (h - sqrtd) / a;

            // check that t is within the (min_t, max_t) range  
            if (!range.contains(t)) {
                t = (h + sqrtd) / a;
                if (!range.contains(t))
                    return false;
            }

            //set rec 
            rec.t = t;
            rec.p = r.at(t);
            auto outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
        }
    private:
        point3 center;
        double radius;
};
#endif