#include "material.h"
#include "project_utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"


int main() {
    // Image
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    
    /** Put all the hittables into the world */
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0, 0.0, -1.0), 0.5, material_right));

    /** Set up camera */
    camera cam;
    cam.aspect_ratio = aspect_ratio;
    cam.image_width = image_width;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vertical_fov = 20;
    cam.position = point3(-2,2,1);
    cam.viewport_position = point3(0,0,-1);
    cam.up = vec3(0,1,0);
    cam.render(world);
}