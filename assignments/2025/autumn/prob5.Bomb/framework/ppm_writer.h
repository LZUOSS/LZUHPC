#ifndef FRAMEWORK_PPM_WRITER_H
#define FRAMEWORK_PPM_WRITER_H

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

static inline void write_ppm_composite(const std::string& filename, int H,
                                       int W, const uint8_t* r_channel,
                                       const uint8_t* g_channel,
                                       const uint8_t* b_channel) {
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    ofs << "P6\n" << W << " " << H << "\n255\n";

    std::vector<uint8_t> pixel(3);
    for (int i = 0; i < H * W; ++i) {
        pixel[0] = r_channel ? r_channel[i] : 0;
        pixel[1] = g_channel ? g_channel[i] : 0;
        pixel[2] = b_channel ? b_channel[i] : 0;
        ofs.write(reinterpret_cast<const char*>(pixel.data()), 3);
    }
}

static inline void write_ppm_grayscale(const std::string& filename,
                                       const uint8_t* data, int H, int W) {
    write_ppm_composite(filename, H, W, data, data, data);
}

#endif