/**
 * @file validate_raw.cpp
 * @brief A high-performance validation tool for comparing raw binary files.
 *
 * This program directly reads two binary files (e.g., terrain.raw) into memory
 * and performs a per-byte approximate comparison. It is designed to be fast
 * and simple, validating optimizations that may introduce acceptable floating-
 * point errors without needing an intermediate text format with hashes.
 *
 * Usage:
 *   ./validate_raw <baseline_file.raw> <your_file.raw>
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <cmath>     // For std::fabs, std::sqrt
#include <algorithm> // For std::max
#include <cstdint>

// ANSI color codes for rich console output
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x0m"

// --- Configuration for Approximate Validation ---
// The maximum allowed absolute difference between two corresponding bytes.
const int MAX_ABS_ERROR_TOLERANCE = 1;

// The maximum percentage of bytes that are allowed to have an error greater than the tolerance.
const double MAX_MISMATCH_RATIO_THRESHOLD = 0.001; // 0.1%

/**
 * @brief Reads an entire binary file into a vector of bytes.
 * @param filepath The path to the binary file.
 * @return A vector<uint8_t> containing the file's content.
 * @throws std::runtime_error if the file cannot be opened or read.
 */
std::vector<uint8_t> read_binary_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read the entire file: " + filepath);
    }
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./validate_raw <baseline_file.raw> <your_file.raw>" << std::endl;
        return 1;
    }

    std::string baseline_filepath = argv[1];
    std::string your_filepath = argv[2];

    try {
        // --- Stage 1: Read both binary files into memory ---
        std::cout << "--- Reading Files ---" << std::endl;
        std::vector<uint8_t> baseline_data = read_binary_file(baseline_filepath);
        std::vector<uint8_t> your_data = read_binary_file(your_filepath);
        std::cout << "  Baseline file size: " << baseline_data.size() << " bytes" << std::endl;
        std::cout << "  Your file size:     " << your_data.size() << " bytes" << std::endl;

        // --- Stage 2: Basic Sanity Check (File Size) ---
        if (baseline_data.size() != your_data.size()) {
            std::cerr << ANSI_COLOR_RED << "\n[FAILURE] File size mismatch. Cannot proceed." << ANSI_COLOR_RESET << std::endl;
            return 1;
        }
        if (baseline_data.empty()) {
             std::cout << ANSI_COLOR_YELLOW << "\n[WARNING] Input files are empty. Validation passed trivially." << ANSI_COLOR_RESET << std::endl;
             return 0;
        }

        // --- Stage 3: Per-Byte Approximate Comparison ---
        
        size_t mismatch_count = 0;
        double max_abs_error = 0.0;
        double sum_sq_error = 0.0;
        size_t first_mismatch_idx = (size_t)-1;

        for (size_t i = 0; i < baseline_data.size(); ++i) {
            double current_abs_error = std::fabs(static_cast<double>(baseline_data[i]) - your_data[i]);
            
            if (current_abs_error > MAX_ABS_ERROR_TOLERANCE) {
                if (mismatch_count == 0) {
                    first_mismatch_idx = i;
                }
                mismatch_count++;
            }

            max_abs_error = std::max(max_abs_error, current_abs_error);
            sum_sq_error += current_abs_error * current_abs_error;
        }

        double mismatch_ratio = static_cast<double>(mismatch_count) / baseline_data.size();
        double rms_error = std::sqrt(sum_sq_error / baseline_data.size());

        // --- Stage 4: Final Report and Verdict ---
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "\n--- Validation Report ---" << std::endl;
        std::cout << "  Max absolute error found: " << max_abs_error << std::endl;
        std::cout << "  Root Mean Square Error (RMSE): " << rms_error << std::endl;
        std::cout << "  Mismatched bytes (error > " << MAX_ABS_ERROR_TOLERANCE << "): " 
                  << mismatch_count << " / " << baseline_data.size() 
                  << " (" << mismatch_ratio * 100.0 << "%)" << std::endl;
        
        std::cout << "\n--- Verdict ---" << std::endl;
        if (mismatch_ratio > MAX_MISMATCH_RATIO_THRESHOLD) {
            std::cout << ANSI_COLOR_RED << "[FAILURE] Mismatch ratio (" << mismatch_ratio * 100.0 
                      << "%) exceeds the allowed threshold of " << MAX_MISMATCH_RATIO_THRESHOLD * 100.0 << "%." << ANSI_COLOR_RESET << std::endl;
            
            std::cout << "\n  First significant mismatch found at byte offset " << first_mismatch_idx << ":" << std::endl;
            std::cout << "    - Expected (baseline): " << static_cast<int>(baseline_data[first_mismatch_idx]) << std::endl;
            std::cout << "    - Got (your output):   " << static_cast<int>(your_data[first_mismatch_idx]) << std::endl;
            return 1;
        } else {
            std::cout << ANSI_COLOR_GREEN << "[SUCCESS] All differences are within acceptable limits. Validation passed." << ANSI_COLOR_RESET << std::endl;
            return 0;
        }

    } catch (const std::exception& e) {
        std::cerr << ANSI_COLOR_RED << "\n[ERROR] An exception occurred: " << e.what() << ANSI_COLOR_RESET << std::endl;
        return 1;
    }
}