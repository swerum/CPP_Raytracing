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

class dielectric : public material {
    public:
        dielectric(double refractive_index) : refractive_index(refractive_index) {}
        bool scatter(const ray& r_in, const hit_details& hit, color& attenuation, ray& scattered) const override {
            double relative_ri = hit.front_face ? 1.0/refractive_index : refractive_index;
            vec3 unit_incoming_direction = unit_vector(r_in.direction());

            /** If we can't refract, then we reflect the ray instead */
            vec3 outgoing_direction;
            if (can_refract(unit_incoming_direction, hit.normal, relative_ri)) {
                outgoing_direction = refract(unit_incoming_direction, hit.normal, relative_ri);
            } else {
                outgoing_direction = reflect(unit_incoming_direction, hit.normal);
            }
            scattered = ray(hit.p, outgoing_direction);
            attenuation = color(1,1,1);
            return true;
        }

    private:
        double refractive_index;

       
        static bool can_refract(vec3 unit_incoming_ray, vec3 normal, double relative_ri) {
             /** 
             * MATH: there are cases when Snell's Law fails, because it returns a refractive angle greater than 90 degrees
             * Since that doesn't make any sense, that means that it literally cannot refract
             */
            double cos_theta = fmin(dot(-unit_incoming_ray, normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            bool cannot_refract = relative_ri * sin_theta > 1.0;
            if (cannot_refract) return false;
            /** Check for Schlick reflectance */
            bool schlick_refracts = schlick_reflectance(cos_theta, relative_ri) <= random_double();
            return schlick_refracts;
        }

        /** 
         * MATH: Returns a value between 0 and 1 depending on the angle of incidence
         * If the angle of incidence is shallow, it returns a high value. 
         * If the reflectance is lower than a random double, it refracts. If it is higher, it reflects
         */
        static double schlick_reflectance(double cosine, double refraction_index) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0*r0;
            double reflectance = r0 + (1-r0)*pow((1 - cosine),5);
            return reflectance;
        }

};

#endif