#include "test_case.h"
#include <iostream>
#include <vector>
#include <string>

#include "reduce.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

// 打印使用帮助
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n";
    std::cout << "Options:\n";
    std::cout << "  -l <size>     Array length (default: 33554432)\n";
    std::cout << "  -t <count>    Number of test runs (default: 10)\n";
    std::cout << "  -h            Show this help message\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " -l 1024000 -t 10\n";
    std::cout << "  " << programName << " -l 2048000 -t 20 \n";
}

int main(int argc, char* argv[]) {
    // 默认参数
    int arrayLength = 1024 * 1024 * 32;
    int testTimes = 10;
    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            if (i + 1 < argc) {
                arrayLength = atoi(argv[++i]);
                if (arrayLength <= 0) {
                    std::cerr << "Error: Array length must be positive!\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: -l requires an argument!\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                testTimes = atoi(argv[++i]);
                if (testTimes <= 0) {
                    std::cerr << "Error: Test times must be positive!\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: -t requires an argument!\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-h") == 0) {
            printUsage(argv[0]);
            return 0;
        }
        else {
            std::cerr << "Error: Unknown option " << argv[i] << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    // 运行测试
    test_reduce(arrayLength, testTimes);
    
    return 0;
}
