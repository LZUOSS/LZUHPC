/**
 * @file harness_perlin.cpp
 * @brief The main test harness for the Perlin noise optimization problem.
 *
 * This program reads configuration from stdin, calls the contestant's
 * `generate_terrain` function, measures its execution time, and validates
 * the result by computing a hash of the output.
 *
 * It now supports two optional command-line arguments:
 *   --visualize <output_filename.ppm>  (for color PPM image)
 *   --output-raw <output_filename.raw> (for raw binary height data)
 *
 * It is NOT intended to be modified by the contestant.
 */

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../framework/hash.h"
#include "../framework/ppm_writer.h"
#include "../framework/timer.h"
#include "../framework/types.h"

#ifndef SUBMISSION_FILE
#define SUBMISSION_FILE "submit_perlin.cpp"
#endif
#include SUBMISSION_FILE

struct RGB {
    uint8_t r, g, b;
};
struct ColorStop {
    uint8_t height;
    RGB color;
};
static inline RGB lerp_color(const RGB& c1, const RGB& c2, float t) {
    return {static_cast<uint8_t>(c1.r + t * (c2.r - c1.r)),
            static_cast<uint8_t>(c1.g + t * (c2.g - c1.g)),
            static_cast<uint8_t>(c1.b + t * (c2.b - c1.b))};
}
RGB get_color_from_map(uint8_t height,
                       const std::vector<ColorStop>& color_map) {
    if (height <= color_map.front().height)
        return color_map.front().color;
    if (height >= color_map.back().height)
        return color_map.back().color;
    for (size_t i = 0; i < color_map.size() - 1; ++i) {
        if (height >= color_map[i].height && height < color_map[i + 1].height) {
            const auto& s1 = color_map[i];
            const auto& s2 = color_map[i + 1];
            float t = static_cast<float>(height - s1.height) /
                      (s2.height - s1.height);
            return lerp_color(s1.color, s2.color, t);
        }
    }
    return color_map.back().color;
}

void read_config_from_stdin(Config& cfg) {
    std::cin >> cfg.H >> cfg.W >> cfg.perlin.octaves >>
        cfg.perlin.persistence >> cfg.perlin.lacunarity >>
        cfg.perlin.base_freq >> cfg.perlin.height_scale >> cfg.seeds.perlin;
    if (std::cin.fail()) {
        throw std::runtime_error("Failed to read configuration from stdin.");
    }
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string visualize_filename = "";
    std::string raw_output_filename = "";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--visualize" && i + 1 < argc) {
            visualize_filename = argv[++i];
        } else if (arg == "--output-raw" && i + 1 < argc) {
            raw_output_filename = argv[++i];
        }
    }

    try {
        Config cfg;
        read_config_from_stdin(cfg);

        std::vector<uint8_t> height_map(static_cast<size_t>(cfg.H) * cfg.W);

        Timer timer;
        timer.tic();
        generate_terrain(height_map.data(), cfg);
        double elapsed_sec = timer.toc();

        if (!raw_output_filename.empty()) {
            std::cerr << "Writing raw height map to " << raw_output_filename
                      << std::endl;
            std::ofstream ofs(raw_output_filename, std::ios::binary);
            if (!ofs) {
                std::cerr << "Error: Cannot open file for raw output: "
                          << raw_output_filename << std::endl;
            } else {
                ofs.write(reinterpret_cast<const char*>(height_map.data()),
                          height_map.size());
            }
        }

        if (!visualize_filename.empty()) {
            std::cerr << "Generating color visualization... saving to "
                      << visualize_filename << std::endl;
            const std::vector<ColorStop> color_map = {
                {30, {10, 20, 60}},     {80, {30, 60, 120}},
                {130, {60, 100, 170}},  {180, {100, 140, 200}},
                {220, {140, 180, 230}}, {255, {180, 210, 250}},
            };
            const size_t num_pixels = height_map.size();
            std::vector<uint8_t> r(num_pixels), g(num_pixels), b(num_pixels);
            for (size_t i = 0; i < num_pixels; ++i) {
                RGB color = get_color_from_map(height_map[i], color_map);
                r[i] = color.r;
                g[i] = color.g;
                b[i] = color.b;
            }
            try {
                write_ppm_composite(visualize_filename, cfg.H, cfg.W, r.data(),
                                    g.data(), b.data());
            } catch (const std::exception& e) {
                std::cerr << "Error writing PPM file: " << e.what()
                          << std::endl;
            }
        }

        uint64_t result_hash = fnv1a64(height_map.data(), height_map.size());
        std::cout << std::hex << std::setfill('0') << std::setw(16)
                  << result_hash << std::endl;
        std::cerr << "time_sec=" << std::fixed << std::setprecision(6)
                  << elapsed_sec << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
