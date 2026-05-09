/**
 * @file harness_splat.cpp
 * @brief The main test harness for the event splatting optimization problem.
 *
 */

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
#define SUBMISSION_FILE "submit_splat.cpp"
#endif
#include SUBMISSION_FILE

struct SplatContext;
SplatContext* splat_create(const Config& cfg);
void splat_build_events(SplatContext* ctx, const Config& cfg);
void splat_apply(SplatContext* ctx,
                 uint8_t* damage_map, uint8_t* occlusion_map,
                 const uint8_t* height_map, const Config& cfg);
void splat_destroy(SplatContext* ctx);
/* --------------------------------------------------------------------------- */

static void read_config_from_stdin(Config& cfg) {
    int gaussian_flag;
    std::cin >> cfg.H >> cfg.W
             >> cfg.bombs.count >> cfg.bombs.r_min >> cfg.bombs.r_max
             >> gaussian_flag
             >> cfg.bullets.count >> cfg.bullets.r_min >> cfg.bullets.r_max
             >> cfg.seeds.events;
    cfg.bombs.gaussian = (gaussian_flag == 1);
    if (std::cin.fail()) {
        throw std::runtime_error("Failed to read configuration from stdin.");
    }
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool do_visualize = false;
    std::string heightmap_filename = "";
    std::string damage_output_filename = "";
    std::string occlusion_output_filename = "";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--visualize") {
            do_visualize = true;
        } else if (arg == "--input-heightmap" && i + 1 < argc) {
            heightmap_filename = argv[++i];
        } else if (arg == "--output-damage-raw" && i + 1 < argc) {
            damage_output_filename = argv[++i];
        } else if (arg == "--output-occlusion-raw" && i + 1 < argc) {
            occlusion_output_filename = argv[++i];
        }
    }

    try {
        Config cfg;
        read_config_from_stdin(cfg);

        const size_t map_size = static_cast<size_t>(cfg.H) * cfg.W;
        std::vector<uint8_t> damage_map(map_size, 0);
        std::vector<uint8_t> occlusion_map(map_size, 0);

        std::vector<uint8_t> height_map;
        if (!heightmap_filename.empty()) {
            std::cerr << "Loading height map from " << heightmap_filename << "..." << std::endl;
            std::ifstream ifs(heightmap_filename, std::ios::binary | std::ios::ate);
            if (!ifs) {
                throw std::runtime_error("Could not open height map file: " + heightmap_filename);
            }
            std::streamsize size = ifs.tellg();
            if (static_cast<size_t>(size) != map_size) {
                throw std::runtime_error("Height map file size mismatch.");
            }
            ifs.seekg(0, std::ios::beg);
            height_map.resize(map_size);
            if (!ifs.read(reinterpret_cast<char*>(height_map.data()), size)) {
                throw std::runtime_error("Could not read height map file.");
            }
        } else {
            std::cerr << "No height map provided, using flat terrain (height 0)." << std::endl;
            height_map.assign(map_size, 0);
        }

        SplatContext* ctx = splat_create(cfg);
        splat_build_events(ctx, cfg);

        Timer timer;
        timer.tic();
        splat_apply(ctx, damage_map.data(), occlusion_map.data(), height_map.data(), cfg);
        double elapsed_sec = timer.toc();

        splat_destroy(ctx);

        if (!damage_output_filename.empty()) {
            std::ofstream ofs(damage_output_filename, std::ios::binary);
            if (!ofs) {
                std::cerr << "Error: Cannot open file for damage raw output: "
                          << damage_output_filename << std::endl;
            } else {
                ofs.write(reinterpret_cast<const char*>(damage_map.data()), map_size);
            }
        }

        if (!occlusion_output_filename.empty()) {
            std::ofstream ofs(occlusion_output_filename, std::ios::binary);
            if (!ofs) {
                std::cerr << "Error: Cannot open file for occlusion raw output: "
                          << occlusion_output_filename << std::endl;
            } else {
                ofs.write(reinterpret_cast<const char*>(occlusion_map.data()), map_size);
            }
        }

        if (do_visualize) {
            std::cerr << "Generating visualization... saving to occlusion_S.ppm" << std::endl;
            try {
                write_ppm_grayscale("occlusion_S.ppm", occlusion_map.data(), cfg.H, cfg.W);
            } catch (const std::exception& e) {
                std::cerr << "Error writing PPM file: " << e.what() << std::endl;
            }
        }

        uint64_t hash = fnv1a64(occlusion_map.data(), map_size);
        std::cout << std::hex << std::setfill('0') << std::setw(16) << hash << std::endl;
        std::cerr << "time_sec=" << std::fixed << std::setprecision(6) << elapsed_sec << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
