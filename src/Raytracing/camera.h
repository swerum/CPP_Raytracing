#ifndef CAMERA_H
#define CAMERA_H

#include "project_utils.h"
#include "hittable.h"


class camera {
    public:
        double aspect_ratio = 1.0; // Ratio of image width over height
        int image_width = 100; // Rendered image width in pixel count

        /* Public Camera Parameters Here */
        void render(const hittable& world) {
            initialize();
            // Render
            std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
            for (int j = 0; j < image_height; j++) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    //every pixel color is defined by a ray going from the camera center to its pixel
                    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                    auto ray_direction = pixel_center - camera_center;
                    ray r(camera_center, ray_direction);
                    color pixel_color = ray_color(r, world);
                    write_color(std::cout, pixel_color);
                }
            }
            std::clog << "\rDone. \n";
        }
    private:
        /* Private Camera Variables Here */
        int image_height;
        vec3 pixel00_loc;
        vec3 pixel_delta_u;
        vec3 pixel_delta_v;
        vec3 camera_center;

        /** Set all the private camera variables */
        void initialize() {
            // Calculate the image height, and ensure that it's at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            // Camera
            auto focal_length = 1.0;
            auto viewport_height = 2.0;
            auto viewport_width = viewport_height * (double(image_width)/image_height);
            camera_center = point3(0, 0, 0);

            // define u and v: the vectors with the direction and length of the viewport axes
            auto viewport_u = vec3(viewport_width, 0, 0);
            auto viewport_v = vec3(0, -viewport_height, 0);

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        }
        
        /** iterate over world objects and display normal as color. if no hit, display blue-white gradient background */
        color ray_color(const ray& r, const hittable& world) const {
            hit_details hit;
            range ray_range = range(0, infinity);
            bool hit_something = world.hits(r, ray_range, hit);
            if (hit_something) {
                auto col = (color(1,1,1)+hit.normal) / 2.0;
                return col;
            }

            //add sky gradient
            auto white = color(1,1,1);
            auto blue = color(0.5, 0.7, 1.0);
            auto dir = r.direction(); //
            dir = dir.normalize();
            auto y = (dir.y() +1)/2;
            return (1-y) * white + y * blue;
        }
};
#endif