#include <array>
#include <charconv>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "geometry/hittable_list.hpp"
#include "geometry/sphere.hpp"
#include "material/material.hpp"
#include "render/camera.hpp"

namespace
{

    // Fixed benchmark scene. Contestants must not change its definition.
    hittable_list build_world()
    {
        begin_scene_random();
        hittable_list world;

        auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

        constexpr double small_radius = 0.2;
        constexpr double gap = 0.001;
        const std::array<point3, 3> large_centers{point3(0, 1, 0), point3(-4, 1, 0), point3(4, 1, 0)};
        std::vector<point3> small_centers;
        small_centers.reserve(22 * 22);
        const auto can_place = [&](const point3 &center)
        {
            const double large_distance = 1.0 + small_radius + gap;
            for(const auto &other : large_centers)
                if((center - other).length_squared() < large_distance * large_distance)
                    return false;
            const double small_distance = 2.0 * small_radius + gap;
            for(const auto &other : small_centers)
                if((center - other).length_squared() < small_distance * small_distance)
                    return false;
            return true;
        };

        for(int a = -11; a < 11; a++)
        {
            for(int b = -11; b < 11; b++)
            {
                auto choose_mat = random_double();
                point3 center{a + 0.9 * random_double(), small_radius, b + 0.9 * random_double()};
                if(!can_place(center))
                    continue;

                shared_ptr<material> sphere_material;
                if(choose_mat < 0.8)
                {
                    const auto first = color::random();
                    auto albedo = first * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                }
                else if(choose_mat < 0.95)
                {
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                }
                else
                    sphere_material = make_shared<dielectric>(1.5);
                world.add(make_shared<sphere>(center, small_radius, sphere_material));
                small_centers.push_back(center);
            }
        }

        auto material1 = make_shared<dielectric>(1.5);
        world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

        auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
        world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

        auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
        world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

        return world;
    }

    bool parse_positive_int(std::string_view text, int &value)
    {
        const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
        return result.ec == std::errc{} && result.ptr == text.data() + text.size() && value > 0;
    }

    void usage(const char *prog)
    {
        std::cerr << "Usage: " << prog << " [width] [samples_per_pixel] [output]\n"
                  << "  width               image width in pixels (default 1200)\n"
                  << "  samples_per_pixel   samples per pixel    (default 200)\n"
                  << "  output              output PPM path      (default out.ppm)\n";
    }

} // namespace

int main(int argc, char *argv[])
{
    // Default configuration used for scoring.
    int image_width = 1200;
    int samples_per_pixel = 200;
    std::string output_path = "out.ppm";

    if(argc > 4)
    {
        usage(argv[0]);
        return 1;
    }
    if((argc > 1 && !parse_positive_int(argv[1], image_width)) ||
       (argc > 2 && !parse_positive_int(argv[2], samples_per_pixel)))
    {
        std::cerr << "Error: width and samples_per_pixel must be positive integers within the int range.\n";
        usage(argv[0]);
        return 1;
    }
    if(argc > 3)
        output_path = argv[3];

    std::ofstream output(output_path, std::ios::out | std::ios::trunc);
    if(!output)
    {
        std::cerr << "Error: cannot open output file: " << output_path << "\n";
        return 1;
    }

    const auto scene_start = std::chrono::steady_clock::now();
    hittable_list world = build_world();
    world.initialize();
    const auto scene_end = std::chrono::steady_clock::now();

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    const auto camera_start = std::chrono::steady_clock::now();
    cam.initialize();
    const auto camera_end = std::chrono::steady_clock::now();

    std::cout << std::fixed << std::setprecision(6)
              << "Scene initialization time: " << std::chrono::duration<double>(scene_end - scene_start).count()
              << " s\n"
              << "Camera initialization time: " << std::chrono::duration<double>(camera_end - camera_start).count()
              << " s\n";

    std::cout << "Rendering " << image_width << "px wide, " << samples_per_pixel << " spp, max depth " << cam.max_depth
              << " -> " << output_path << "\n"
              << std::flush;

    auto start = std::chrono::steady_clock::now();
    cam.render(world, output);
    output.close();
    auto end = std::chrono::steady_clock::now();
    if(!output)
    {
        std::cerr << "Error: failed to write output file: " << output_path << "\n";
        return 1;
    }

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render time: " << std::fixed << std::setprecision(3) << elapsed.count() << " s\n";

    return 0;
}
