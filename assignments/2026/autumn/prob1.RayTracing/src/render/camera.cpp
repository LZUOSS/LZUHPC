#include <cmath>
#include <iostream>

#include "material/material.hpp"
#include "math/color.hpp"
#include "render/camera.hpp"

void camera::render(const hittable &world, std::ostream &out)
{
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = 0; j < image_height; j++)
    {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

        for(int i = 0; i < image_width; i++)
        {
            begin_pixel_random(static_cast<std::uint64_t>(j) * static_cast<std::uint64_t>(image_width) +
                               static_cast<std::uint64_t>(i));
            color pixel_color(0, 0, 0);
            for(int sample = 0; sample < samples_per_pixel; sample++)
            {
                ray r = get_ray(i, j);
                pixel_color += ray_color(r, max_depth, world);
            }
            line[i] = pixel_samples_scale * pixel_color;
        }
        for(int i = 0; i < image_width; i++)
        {
            write_color(out, line[i]);
        }
    }

    std::clog << "\rDone.                 \n";
}

void camera::initialize()
{
    line.resize(image_width);
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = lookfrom;
    pixel_samples_scale = 1.0 / samples_per_pixel;

    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    vec3 viewport_u = viewport_width * u;
    vec3 viewport_v = viewport_height * -v; // Image rows run downward.

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
}

ray camera::get_ray(int i, int j) const
{
    // Jitter the pixel target and sample the lens for depth of field.
    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
}

vec3 camera::sample_square() const
{
    // Braced initialization fixes the order of random draws (x, then y).
    return vec3{random_double() - 0.5, random_double() - 0.5, 0};
}

point3 camera::defocus_disk_sample() const
{
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

color camera::ray_color(const ray &r, int depth, const hittable &world) const
{
    if(depth <= 0)
        return color(0, 0, 0);
    hit_record rec;

    if(world.hit(r, interval(0.001, infinity), rec))
    {
        ray scattered;
        color attenuation;
        if(rec.mat->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, depth - 1, world);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}
