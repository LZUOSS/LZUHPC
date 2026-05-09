/**
 * @file baseline_splat.cpp
 * @brief Reference implementation used for benchmarking and validation.
 *
 * Kept immutable to provide a stable baseline. Contestants should optimize
 * `submit_splat.cpp` and compare against this file using the provided tools.
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include "../framework/prng.h"
#include "../framework/types.h"

struct SplatContext {
    struct Event {
        float x, y;
        float rx, ry;
        float intensity;
        uint8_t type;  // 0: bullet, 1: bomb
    };
    std::vector<Event> events;
};

static inline uint8_t saturating_add_u8(uint8_t a, int b) {
    int sum = static_cast<int>(a) + b;
    if (sum > 255) return 255;
    if (sum < 0) return 0;
    return static_cast<uint8_t>(sum);
}

SplatContext* splat_create(const Config& cfg) {
    (void)cfg;
    auto* ctx = new SplatContext();
    return ctx;
}

void splat_build_events(SplatContext* ctx, const Config& cfg) {
    ctx->events.clear();
    ctx->events.reserve(static_cast<size_t>(cfg.bombs.count + cfg.bullets.count));

    XorShift64 rng(cfg.seeds.events);

    for (int i = 0; i < cfg.bombs.count; ++i) {
        ctx->events.push_back({static_cast<float>(rng.uniform_double() * (cfg.W - 1)),
                               static_cast<float>(rng.uniform_double() * (cfg.H - 1)),
                               static_cast<float>(rng.uniform_int(cfg.bombs.r_min, cfg.bombs.r_max)),
                               static_cast<float>(rng.uniform_int(cfg.bombs.r_min, cfg.bombs.r_max)),
                               40.0f, 1});
    }
    for (int i = 0; i < cfg.bullets.count; ++i) {
        ctx->events.push_back({static_cast<float>(rng.uniform_double() * (cfg.W - 1)),
                               static_cast<float>(rng.uniform_double() * (cfg.H - 1)),
                               static_cast<float>(rng.uniform_int(cfg.bullets.r_min, cfg.bullets.r_max)),
                               static_cast<float>(rng.uniform_int(cfg.bullets.r_min, cfg.bullets.r_max)),
                               10.0f, 0});
    }
}

void splat_apply(SplatContext* ctx, uint8_t* damage_map, uint8_t* occlusion_map,
                 const uint8_t* height_map, const Config& cfg) {
    for (const auto& ev : ctx->events) {
        const int x_min = static_cast<int>(std::max(0.0f, std::floor(ev.x - ev.rx)));
        const int x_max =
            static_cast<int>(std::min(static_cast<float>(cfg.W - 1), std::ceil(ev.x + ev.rx)));
        const int y_min = static_cast<int>(std::max(0.0f, std::floor(ev.y - ev.ry)));
        const int y_max =
            static_cast<int>(std::min(static_cast<float>(cfg.H - 1), std::ceil(ev.y + ev.ry)));

        const float inv_rx_sq = 1.0f / (ev.rx * ev.rx + 1e-6f);
        const float inv_ry_sq = 1.0f / (ev.ry * ev.ry + 1e-6f);

        for (int y = y_min; y <= y_max; ++y) {
            const float dy = (static_cast<float>(y) + 0.5f) - ev.y;
            const float ky = (dy * dy) * inv_ry_sq;

            float dx = (static_cast<float>(x_min) + 0.5f) - ev.x;
            int idx = y * cfg.W + x_min;

            for (int x = x_min; x <= x_max; ++x, ++idx) {
                const float dist_sq = (dx * dx) * inv_rx_sq + ky;

                if (dist_sq <= 1.0f) {
                    int damage_add = 0;
                    int occlusion_add = 0;

                    if (ev.type == 1) {
                        if (cfg.bombs.gaussian) {
                            damage_add = static_cast<int>(std::round(ev.intensity * expf(-3.0f * dist_sq)));
                        } else {
                            damage_add = static_cast<int>(std::round(ev.intensity * (1.0f - sqrtf(dist_sq))));
                        }
                        occlusion_add = (damage_add > 0) ? 2 : 0;
                    } else {
                        damage_add = static_cast<int>(std::round(ev.intensity));
                        occlusion_add = 1;
                    }

                    const uint8_t h = height_map[idx];
                    const float attenuation_factor = 1.0f - (static_cast<float>(h) / 255.0f) * 0.8f;
                    damage_add = static_cast<int>(static_cast<float>(damage_add) * attenuation_factor);

                    damage_map[idx] = saturating_add_u8(damage_map[idx], damage_add);
                    occlusion_map[idx] = saturating_add_u8(occlusion_map[idx], occlusion_add);
                }

                dx += 1.0f;
            }
        }
    }
}

void splat_destroy(SplatContext* ctx) {
    delete ctx;
}

