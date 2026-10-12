#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <random>

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Benchmark contract: do not change the engine, seed derivation or draw order.
constexpr unsigned int rt_seed = 42u;

struct random_state
{
    std::mt19937 generator{rt_seed};
    std::uniform_real_distribution<double> distribution{0.0, 1.0};
};

inline thread_local random_state scene_random_state;
inline thread_local random_state pixel_random_state;
// Existing sampling helpers use the stream selected for this thread.
inline thread_local random_state *active_random_state = &scene_random_state;

inline void begin_scene_random()
{
    scene_random_state.generator.seed(rt_seed);
    scene_random_state.distribution.reset();
    active_random_state = &scene_random_state;
}

inline void begin_pixel_random(std::uint64_t pixel_index)
{
    // Seed once per pixel so scheduling does not affect its samples.
    std::seed_seq seed{rt_seed, static_cast<unsigned int>(pixel_index & 0xffffffffULL),
                       static_cast<unsigned int>(pixel_index >> 32)};
    pixel_random_state.generator.seed(seed);
    pixel_random_state.distribution.reset();
    active_random_state = &pixel_random_state;
}

inline double degrees_to_radians(double degrees)
{
    return degrees * pi / 180.0;
}

inline double random_double()
{
    return active_random_state->distribution(active_random_state->generator);
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}
