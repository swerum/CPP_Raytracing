#ifndef COLOR_H
#define COLOR_H

#include "project_utils.h"
#include "vec3.h"
#include "range.h"

using color = vec3;

double linear_to_gamma(double linear_component) {
    if (linear_component <= 0) return 0;
    return sqrt(linear_component);
}
color linear_to_gamma(color linear_component) {
    auto r = linear_to_gamma(linear_component.x());
    auto g = linear_to_gamma(linear_component.y());
    auto b = linear_to_gamma(linear_component.z());
    return color(r,g,b);
}

void write_color(std::ostream& out, const color& pixel_color) {
    color gamma_color = linear_to_gamma(pixel_color);

    auto r = gamma_color.x();
    auto g = gamma_color.y();
    auto b = gamma_color.z();

    // Translate the [0,1] component values to the byte range [0,255].
    static const range intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif