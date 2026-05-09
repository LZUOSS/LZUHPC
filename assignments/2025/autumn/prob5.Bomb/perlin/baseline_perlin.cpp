/**
 * @file baseline_perlin.cpp
 * @brief Reference implementation used for benchmarking and validation.
 *
 * Kept immutable to provide a stable baseline. Contestants should optimize
 * `submit_perlin.cpp` and compare against this file using the provided tools.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <vector>

#include "../framework/prng.h"
#include "../framework/types.h"

namespace PerlinBaseline {
class PerlinNoiseGenerator {
   public:
    explicit PerlinNoiseGenerator(uint64_t seed) {
        p.resize(512);
        std::iota(p.begin(), p.begin() + 256, 0);
        XorShift64 rng(seed);
        for (int i = 255; i > 0; --i) {
            std::swap(p[i], p[rng.uniform_int(0, i)]);
        }
        for (int i = 0; i < 256; ++i) {
            p[i + 256] = p[i];
        }
    }
    double noise(double x, double y) const {
        const int X = static_cast<int>(std::floor(x)) & 255;
        const int Y = static_cast<int>(std::floor(y)) & 255;
        const double xf = x - std::floor(x);
        const double yf = y - std::floor(y);
        const double u = fade(xf);
        const double v = fade(yf);
        const int aa = p[p[X] + Y], ab = p[p[X] + Y + 1], ba = p[p[X + 1] + Y],
                  bb = p[p[X + 1] + Y + 1];
        const double res =
            lerp(lerp(grad(aa, xf, yf), grad(ba, xf - 1, yf), u),
                 lerp(grad(ab, xf, yf - 1), grad(bb, xf - 1, yf - 1), u), v);
        return (res + 1.0) / 2.0;
    }

   private:
    std::vector<int> p;
    static inline double fade(double t) {
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    }
    static inline double lerp(double a, double b, double t) {
        return a + t * (b - a);
    }
    static inline double grad(int hash, double x, double y) {
        switch (hash & 3) {
            case 0:
                return x + y;
            case 1:
                return -x + y;
            case 2:
                return x - y;
            case 3:
                return -x - y;
            default:
                return 0;
        }
    }
};
}  // namespace PerlinBaseline

namespace Erosion {

struct Droplet {
    float posX, posY;
    float dirX, dirY;
    float velocity;
    float water;
    float sediment;
};

class ErosionSimulator {
   public:
    const int numIterations = 50000;
    const int maxLifetime = 30;
    const float inertia = 0.05f;
    const float sedimentCapacityFactor = 4.0f;
    const float minSlope = 0.01f;
    const float erodeSpeed = 0.3f;
    const float depositSpeed = 0.3f;
    const float evaporateSpeed = 0.01f;
    const float gravity = 4.0f;
    const float initialWater = 1.0f;
    const float initialSpeed = 1.0f;

    void erode(std::vector<float>& map, int H, int W, uint64_t seed) {
        XorShift64 rng(seed);

        for (int i = 0; i < numIterations; ++i) {
            Droplet droplet = {
                .posX = static_cast<float>(rng.uniform_double() * (W - 1)),
                .posY = static_cast<float>(rng.uniform_double() * (H - 1)),
                .dirX = 0,
                .dirY = 0,
                .velocity = initialSpeed,
                .water = initialWater,
                .sediment = 0};

            for (int j = 0; j < maxLifetime; ++j) {
                int nodeX = static_cast<int>(droplet.posX);
                int nodeY = static_cast<int>(droplet.posY);

                if (nodeX < 0 || nodeX >= W - 1 || nodeY < 0 ||
                    nodeY >= H - 1) {
                    break;
                }

                float u = droplet.posX - nodeX;
                float v = droplet.posY - nodeY;
                int idx = nodeY * W + nodeX;
                float height_NW = map[idx];
                float height_NE = map[idx + 1];
                float height_SW = map[idx + W];
                float height_SE = map[idx + W + 1];
                float gradX = (height_NE - height_NW) * (1 - v) +
                              (height_SE - height_SW) * v;
                float gradY = (height_SW - height_NW) * (1 - u) +
                              (height_SE - height_NE) * u;

                droplet.dirX = (droplet.dirX * inertia - gradX * (1 - inertia));
                droplet.dirY = (droplet.dirY * inertia - gradY * (1 - inertia));

                float len = std::sqrt(droplet.dirX * droplet.dirX +
                                      droplet.dirY * droplet.dirY);
                if (len > 1e-6f) {
                    droplet.dirX /= len;
                    droplet.dirY /= len;
                }
                droplet.velocity =
                    std::sqrt(droplet.velocity * droplet.velocity +
                              std::abs(gradY) * gravity);  // Simplified physics

                float sedimentCapacity = std::max(-gradY, minSlope) *
                                         droplet.velocity * droplet.water *
                                         sedimentCapacityFactor;

                if (droplet.sediment > sedimentCapacity || gradY > 0) {
                    float amountToDeposit =
                        (gradY > 0) ? std::min(droplet.sediment, depositSpeed)
                                    : (droplet.sediment - sedimentCapacity) *
                                          depositSpeed;
                    droplet.sediment -= amountToDeposit;

                    map[idx] += amountToDeposit * (1 - u) * (1 - v);
                    map[idx + 1] += amountToDeposit * u * (1 - v);
                    map[idx + W] += amountToDeposit * (1 - u) * v;
                    map[idx + W + 1] += amountToDeposit * u * v;
                } else {
                    float amountToErode = std::min(
                        (sedimentCapacity - droplet.sediment) * erodeSpeed,
                        -gradY);

                    float w_nw = (1 - u) * (1 - v);
                    float w_ne = u * (1 - v);
                    float w_sw = (1 - u) * v;
                    float w_se = u * v;
                    map[idx] -= amountToErode * w_nw;
                    map[idx + 1] -= amountToErode * w_ne;
                    map[idx + W] -= amountToErode * w_sw;
                    map[idx + W + 1] -= amountToErode * w_se;
                    droplet.sediment += amountToErode;
                }

                droplet.posX += droplet.dirX;
                droplet.posY += droplet.dirY;
                droplet.water *= (1 - evaporateSpeed);
            }
        }
    }
};

}  // namespace Erosion

void generate_terrain(uint8_t* height_map, const Config& cfg) {
    PerlinBaseline::PerlinNoiseGenerator generator(cfg.seeds.perlin);
    const double SHAPING_EXPONENT = 1.5;
    PerlinBaseline::PerlinNoiseGenerator warp_generator(cfg.seeds.perlin + 1);
    const double WARP_FREQUENCY = 0.005;
    const double WARP_AMPLITUDE = 60.0;

    std::vector<float> float_map(static_cast<size_t>(cfg.H) * cfg.W);

    for (int y = 0; y < cfg.H; ++y) {
        for (int x = 0; x < cfg.W; ++x) {
            double q1 =
                warp_generator.noise(x * WARP_FREQUENCY, y * WARP_FREQUENCY);
            double q2 = warp_generator.noise((x + 123.4) * WARP_FREQUENCY,
                                             (y + 567.8) * WARP_FREQUENCY);
            double warped_x =
                static_cast<double>(x) + WARP_AMPLITUDE * (2.0 * q1 - 1.0);
            double warped_y =
                static_cast<double>(y) + WARP_AMPLITUDE * (2.0 * q2 - 1.0);

            double total_noise = 0.0, amplitude = 1.0,
                   frequency = cfg.perlin.base_freq, max_amplitude = 0.0;
            for (int o = 0; o < cfg.perlin.octaves; ++o) {
                total_noise += generator.noise(warped_x * frequency,
                                               warped_y * frequency) *
                               amplitude;
                max_amplitude += amplitude;
                amplitude *= cfg.perlin.persistence;
                frequency *= cfg.perlin.lacunarity;
            }
            if (max_amplitude > 0.0) {
                total_noise /= max_amplitude;
            }
            total_noise = std::max(0.0, std::min(1.0, total_noise));
            total_noise = pow(total_noise, SHAPING_EXPONENT);
            float_map[y * cfg.W + x] = static_cast<float>(total_noise);
        }
    }

    Erosion::ErosionSimulator erosion_sim;
    erosion_sim.erode(float_map, cfg.H, cfg.W, cfg.seeds.events);

    const float FIXED_MIN_H = 0.0f;
    const float FIXED_MAX_H = 1.0f;
    const float FIXED_RANGE = FIXED_MAX_H - FIXED_MIN_H;

    for (size_t i = 0; i < float_map.size(); ++i) {
        float normalized = (float_map[i] - FIXED_MIN_H) / FIXED_RANGE;
        height_map[i] = static_cast<uint8_t>(
            std::max(0.0f, std::min(normalized * 255.0f, 255.0f)));
    }
}

