#pragma once

#include <vector>

#include "geometry/hittable.hpp"

class camera
{
public:
    double aspect_ratio = 1.0; // Width / height
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;

    double vfov = 90; // Vertical field of view in degrees
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0; // Lens aperture angle in degrees; zero disables defocus
    double focus_dist = 10;   // Distance to the focus plane

    // Call after setting camera parameters, before starting the render timer.
    void initialize();
    void render(const hittable &world, std::ostream &out);

private:
    int image_height;
    double pixel_samples_scale;
    point3 center;
    point3 pixel00_loc; // Upper-left pixel center
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w; // Camera basis: right, up, backward
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    std::vector<color> line;

    ray get_ray(int i, int j) const;
    vec3 sample_square() const;
    point3 defocus_disk_sample() const;
    color ray_color(const ray &r, int depth, const hittable &world) const;
};
