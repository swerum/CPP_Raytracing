#ifndef CAMERA_H
#define CAMERA_H

#include "project_utils.h"
#include "hittable.h"
#include "material.h"


class camera {
    public:
        double aspect_ratio = 1.0; // Ratio of image width over height
        int image_width = 100; // Rendered image width in pixel count
        int samples_per_pixel = 1;
        double max_depth = 10;
        double vertical_fov = 90; /** Vertical field of view */

        /** Variables defining the Transform (location, rotation) */
        point3 position; /** Where is the camera in scene space */
        point3 viewport_position; /** Where is the camera looking */
        vec3 up; /** what is up for the camera. Affects rotation of the scene */

        double defocus_angle = 0; // Variation angle of rays through each pixel. Defines the size of the "lens" instead of giving it a radius
        double focus_dist = 10; // Distance from camera position to plane of perfect focus. Here: same as focal length

        /* Public Camera Parameters Here */
        void render(const hittable& world) {
            initialize();
            // Render
            std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
            for (int j = 0; j < image_height; j++) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    //every pixel color is defined by a ray going from the camera center to its pixel
                    color pixel_color = get_pixel_color(i,j, world);
                    write_color(std::cout, pixel_color);
                }
            }
            std::clog << "\rDone. \n";
        }
    private:
        /* Private Camera Variables Here */
        int image_height;
        vec3 pixel00_loc;
        vec3 pixel_width_vector;
        vec3 pixel_height_vector;
        vec3 camera_center;
        vec3 local_x_direction,local_y_direction,local_z_direction; /** local coordinate system unit vectors */
        double lens_radius;
        vec3 defocus_disk_u; // Defocus disk horizontal radius
        vec3 defocus_disk_v; // Defocus disk vertical radius

        /** Set all the private camera variables */
        void initialize() {
            // Calculate the image height, and ensure that it's at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            /** 
             * We use the focal length and vertical FOV to calculate the viewport height. 
             * We use the viewport height and image's aspect ratio to set the viewport width
            */
            auto vfov_radians = degrees_to_radians(vertical_fov);
            auto viewport_height = tan(vfov_radians / 2) * 2.0 * focus_dist;
            auto viewport_width = viewport_height * (double(image_width)/image_height);
            camera_center = position;

            // Calculate the local_x_direction,local_y_direction,local_z_direction unit basis vectors for the camera coordinate frame.
            local_z_direction = unit_vector(position - viewport_position);
            local_x_direction = unit_vector(cross(up, local_z_direction));
            local_y_direction = cross(local_z_direction, local_x_direction);

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            auto viewport_width_vector = viewport_width * local_x_direction;
            auto viewport_heigth_vector = -viewport_height * local_y_direction;
            pixel_width_vector = viewport_width_vector / image_width;
            pixel_height_vector = viewport_heigth_vector / image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = camera_center - focus_dist * local_z_direction - viewport_width_vector/2 - viewport_heigth_vector/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_width_vector + pixel_height_vector);

            auto lens_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = lens_radius * local_x_direction;
            defocus_disk_v = lens_radius * local_y_direction;
        }

        color get_pixel_color(int x, int y, const hittable& world) {
            color pixel_color = color(0,0,0);
            /**Anti-aliasing: we slightly randomize the starting position within the pixel
             *  and take the average of all the colors we get back */
            for (int i = 0; i < samples_per_pixel; i++)
            {
                auto pixel_center = pixel00_loc + (x * pixel_width_vector) + (y * pixel_height_vector);
                auto sample_point = pixel_center + random_double(-0.5, 0.5) * pixel_width_vector + random_double(-0.5, 0.5) * pixel_height_vector;
                auto ray_origin = defocus_angle < 0 ? camera_center : sample_on_lens();
                auto ray_direction = sample_point - ray_origin;
                ray r(ray_origin, ray_direction) ;
                pixel_color += ray_color(r, world, max_depth);
            }
            pixel_color /= samples_per_pixel;
            return pixel_color;
        }
        
        /** iterate over world objects and display normal as color. if no hit, display blue-white gradient background */
        color ray_color(const ray& r, const hittable& world, double depth) const {
            if (depth <= 0) return color(0,0,0);

            hit_details hit;
            /**
             * NOTE: Shadow Acne 
             * if min is zero, we might hit the same geometry again by accident due to rounding errors 
             * This makes the geometry darker and have weird bright spots
             * */
            range ray_range = range(0.001, infinity); 
            bool hit_something = world.hits(r, ray_range, hit);
            if (hit_something) {
                ray outgoing_ray;
                color attenuation;
                if (hit.mat->scatter(r, hit, attenuation, outgoing_ray)) {
                    return attenuation * ray_color(outgoing_ray, world, depth-1);
                }
                return color(0,0,0);
            }

            //add sky gradient
            auto white = color(1,1,1);
            auto blue = color(0.5, 0.7, 1.0);
            auto dir = unit_vector(r.direction());
            auto y = (dir.y() +1)/2;
            return (1-y) * white + y * blue;
        }

        point3 sample_on_lens() const {
            vec3 r = random_in_unit_disk();
            vec3 offset =  r.x() * defocus_disk_u + r.y() * defocus_disk_v;
            return camera_center + offset;
        }
};
#endif