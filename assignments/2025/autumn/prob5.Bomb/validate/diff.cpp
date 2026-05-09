/**
 * @file diff.cpp
 * @brief To detect the difference between visualizations. 
 * 
 * USAGE
 * > g++ diff.cpp -o diff
 * > ./diff <ppm-file 1> <ppm-file 2>
 * 
 * OUTPUT
 * > Dimention
 * > Mismatched Pixels
 * > Max Absolute Error
 * > RMSE
 * > PSNR
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ANSI color codes
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct RGB {
    unsigned char r, g, b;
};

struct PPMImage {
    int width = 0, height = 0;
    std::vector<RGB> data;
};

PPMImage loadImage(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open file: " + filepath);

    std::string magic;
    file >> magic;
    if (magic != "P6")
        throw std::runtime_error("Invalid PPM format. Only P6 is supported.");

    PPMImage img;
    int maxval;
    file >> img.width >> img.height >> maxval;
    if (maxval != 255)
        throw std::runtime_error("Only 8-bit PPM (maxval 255) is supported.");

    file.get();  // Consume the single whitespace character

    img.data.resize(img.width * img.height);
    file.read(reinterpret_cast<char*>(img.data.data()),
              img.data.size() * sizeof(RGB));
    if (!file)
        throw std::runtime_error("Failed to read pixel data from " + filepath);

    return img;
}

void saveDiffImage(const std::string& filepath, int width, int height,
                   const std::vector<unsigned char>& diff_data) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << ANSI_COLOR_YELLOW << "Warning: Could not open " << filepath
                  << " for writing diff image." << ANSI_COLOR_RESET
                  << std::endl;
        return;
    }
    file << "P6\n" << width << " " << height << "\n255\n";
    for (unsigned char val : diff_data) {
        file.put(val);
        file.put(val);
        file.put(val);  // Write as grayscale
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./diff_ppm <baseline_image.ppm> <your_image.ppm>"
                  << std::endl;
        return 1;
    }

    try {
        std::cout << "--- Comparing PPM Files ---" << std::endl;
        PPMImage baseline = loadImage(argv[1]);
        PPMImage your_image = loadImage(argv[2]);

        if (baseline.width != your_image.width ||
            baseline.height != your_image.height) {
            throw std::runtime_error("Image dimensions do not match!");
        }

        long long mismatch_pixels = 0;
        double sum_sq_error = 0;
        int max_abs_error = 0;
        const size_t num_pixels = baseline.data.size();
        std::vector<unsigned char> diff_visual(num_pixels);

        for (size_t i = 0; i < num_pixels; ++i) {
            int err_r =
                static_cast<int>(baseline.data[i].r) - your_image.data[i].r;
            int err_g =
                static_cast<int>(baseline.data[i].g) - your_image.data[i].g;
            int err_b =
                static_cast<int>(baseline.data[i].b) - your_image.data[i].b;

            if (err_r != 0 || err_g != 0 || err_b != 0) {
                mismatch_pixels++;
            }

            sum_sq_error += static_cast<double>(err_r * err_r + err_g * err_g +
                                                err_b * err_b);

            int current_max_abs_err =
                std::max({std::abs(err_r), std::abs(err_g), std::abs(err_b)});
            if (current_max_abs_err > max_abs_error) {
                max_abs_error = current_max_abs_err;
            }
            diff_visual[i] = static_cast<unsigned char>(std::min(
                255, current_max_abs_err * 16));  // Scale error for visibility
        }

        // --- Calculate Metrics ---
        double mse = sum_sq_error / (num_pixels * 3.0);
        double rmse = std::sqrt(mse);
        double psnr =
            (mse > 1e-9) ? 10.0 * log10((255.0 * 255.0) / mse) : 99.99;

        // --- Print Report ---
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "--- Image Difference Report ---" << std::endl;
        std::cout << "Dimensions: " << baseline.width << "x" << baseline.height
                  << std::endl;
        std::cout << "Mismatched Pixels: " << mismatch_pixels << " / "
                  << num_pixels << " ("
                  << (static_cast<double>(mismatch_pixels) / num_pixels * 100.0)
                  << "%)" << std::endl;
        std::cout << "Max Absolute Error (per channel): " << max_abs_error
                  << std::endl;
        std::cout << "Root Mean Square Error (RMSE): " << rmse << std::endl;
        std::cout << "Peak Signal-to-Noise Ratio (PSNR): ";
        if (psnr > 80)
            std::cout << ANSI_COLOR_GREEN;
        else if (psnr > 40)
            std::cout << ANSI_COLOR_YELLOW;
        else
            std::cout << ANSI_COLOR_RED;
        std::cout << psnr << " dB" << ANSI_COLOR_RESET << std::endl;

        saveDiffImage("diff.ppm", baseline.width, baseline.height, diff_visual);
        std::cout << "\nVisual difference map saved to 'diff.ppm'."
                  << std::endl;

    } catch (const std::exception& e) {
        std::cerr << ANSI_COLOR_RED << "Error: " << e.what() << ANSI_COLOR_RESET
                  << std::endl;
        return 1;
    }

    return 0;
}