#include "color.h"
#include "ray.h"
#include "vec3.h"
#include <iostream>

bool hit_sphere(const point3& center, double radius, const ray& r) {
    //quadratic formula parameters
    auto a = dot(r.direction(), r.direction());
    auto c_minus_q = center - r.origin();
    auto b = dot(-2 * r.direction(), c_minus_q);
    auto c = dot(c_minus_q, c_minus_q) - radius*radius;
    /** we only need the quadratic formula discriminant, because we're only 
     * interested if a solution to the formula exists, not what the solution is
    */
    auto discriminant = b*b - 4*a*c; 
    return discriminant >= 0;
}

color ray_color(const ray& r) {
    if (hit_sphere(point3(0,0,-1), 0.5, r))
        return color(1, 0, 0);

    //add gradient
    auto white = color(1,1,1);
    auto blue = color(0.5, 0.7, 1.0);
    auto dir = r.direction(); //
    dir = dir.normalize();
    auto y = (dir.y() +1)/2;
    return (1-y) * white + y * blue;
}


int main() {
    // Image
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    // Calculate the image height, and ensure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width)/image_height);
    auto camera_center = point3(0, 0, 0);

    // define u and v: the vectors with the direction and length of the viewport axes
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            //every pixel color is defined by a ray going from the camera center to its pixel
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone. \n";
}