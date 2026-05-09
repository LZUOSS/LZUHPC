#include <iostream>
#include "test_case.h"
#include <cstdlib>
#include <string>

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -m <value>     Number of rows in matrix A (default: 2048)" << std::endl;
    std::cout << "  -n <value>     Number of columns in matrix B (default: 2048)" << std::endl;
    std::cout << "  -k <value>     Number of columns in  matrix A / rows in matrix B (default: 2048)" << std::endl;
    std::cout << "  -t <value>     Number of test iterations (default: 5)" << std::endl;
    std::cout << "  -h, --help     Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " -m 1024 -n 1024 -k 1024 -t 10 " << std::endl;
    std::cout << "  " << program_name << " -m 4096 -k 2048" << std::endl;
    std::cout << "  " << program_name << "  # Use all default values" << std::endl;
}

int main(int argc, char* argv[]) {
    // 默认参数
    int m = 2048, n = 2048, k = 2048, test_times = 5;
    
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        }
        else if (arg == "-m") {
            if (i + 1 < argc) {
                m = std::atoi(argv[++i]);
                if (m <= 0) {
                    std::cerr << "Error: m must be a positive integer" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -m requires a value" << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
        else if (arg == "-n") {
            if (i + 1 < argc) {
                n = std::atoi(argv[++i]);
                if (n <= 0) {
                    std::cerr << "Error: n must be a positive integer" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -n requires a value" << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
        else if (arg == "-k") {
            if (i + 1 < argc) {
                k = std::atoi(argv[++i]);
                if (k <= 0) {
                    std::cerr << "Error: k must be a positive integer" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -k requires a value" << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
        else if (arg == "-t") {
            if (i + 1 < argc) {
                test_times = std::atoi(argv[++i]);
                if (test_times <= 0) {
                    std::cerr << "Error: test_times must be a positive integer" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -t requires a value" << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        }
        else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    // 打印测试参数
    std::cout << "Matrix dimensions: " << m << " x " << k << " (A) * " << k << " x " << n << " (B)" ;
    std::cout << "Test iterations: " << test_times<< std::endl;
    test_gemm_cpu(m, n, k,test_times);
    
    return 0;
}
