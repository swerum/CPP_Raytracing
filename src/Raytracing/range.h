#include "project_utils.h"

#ifndef INTERVAL_H
#define INTERVAL_H

class range {
    public:
        double min, max;
        range() : min(+infinity), max(-infinity) {} // Default interval is empty
        range(double min, double max) : min(min), max(max) {}
        
        double size() const {
            return max - min;
        }
        bool contains(double x) const {
            return min <= x && x <= max;
        }
        bool surrounds(double x) const {
            return min < x && x < max;
        }
        static const range empty, universe;

        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }
};
const range range::empty = range(+infinity, -infinity);
const range range::universe = range(-infinity, +infinity);
#endif