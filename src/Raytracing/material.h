#ifndef MATERIAL_H
#define MATERIAL_H

#include "project_utils.h"
#include "hittable.h"
// class hit_details;  //forward declaration of hit_details

class material {
    public:
        virtual ~material() = default;

        /** given the incoming ray and the  hit details, 
         * this function sets the outgoing ray and color */
        virtual bool scatter(const ray& r_in, const hit_details& hit, color& attenuation, ray& scattered) const {
            return false;
        }
};

/** NOTE: Very important to specify public inheritance or main won't know that lambertian is a material */
class lambertian : public material {
    public:
        lambertian(const color& albedo) : albedo(albedo) {}

        /** NOTE: method with const keyword makes it so that class properties cannot be changed */
        /** NOTE: const parameters cannot be changed --> all others are being changed :/ */
        bool scatter(const ray& r_in, const hit_details& hit, color& attenuation, ray& scattered) const override {
            auto scatter_direction = hit.normal + random_on_hemisphere(hit.normal);
            if (scatter_direction.near_zero()) scatter_direction = hit.normal;

            scattered = ray(hit.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    private:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_details& hit, color& attenuation, ray& scattered) const override {
            vec3 scatter_direction = reflect(r_in.direction(), hit.normal);
            scatter_direction = unit_vector(scatter_direction) + random_unit_vector() * fuzz;
            scattered = ray(hit.p, scatter_direction);
            attenuation = albedo;

            //check if the direction is back into the surface, in which case we consider the ray absorbed --> return false;
            bool scatter_toward_normal = dot(scatter_direction, hit.normal) > 0;
            return scatter_toward_normal;
        }
    private:
        color albedo;
        double fuzz;
};
#endif