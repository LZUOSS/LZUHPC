#ifndef FRAMEWORK_TYPES_H
#define FRAMEWORK_TYPES_H

#include <vector>

struct Config {
    int H = 1024;
    int W = 1024;

    struct PerlinConfig {
        int octaves = 4;
        double persistence = 0.5;
        double lacunarity = 2.0;
        double base_freq = 0.015625;
        int height_scale = 40;
    } perlin;

    struct BombConfig {
        int count = 3000;
        int r_min = 6;
        int r_max = 32;
        bool gaussian = true;
    } bombs;

    struct BulletConfig {
        int count = 200000;
        int r_min = 1;
        int r_max = 3;
    } bullets;

    struct SeedConfig {
        uint64_t events = 123456789;
        uint64_t perlin = 987654321;
    } seeds;
};

struct SceneBuffers {
    std::vector<uint8_t> height_map;
    std::vector<uint8_t> damage_map;
    std::vector<uint8_t> occlusion_map;

    SceneBuffers(int H, int W) {
        height_map.resize(H * W);
        damage_map.assign(H * W, 0);
        occlusion_map.assign(H * W, 0);
    }
};

#endif