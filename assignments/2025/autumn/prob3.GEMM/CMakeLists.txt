cmake_minimum_required(VERSION 3.10)
project(Gemm CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 包含头文件目录
include_directories(include)
# 查找OpenMP包
find_package(OpenMP REQUIRED)

# 包含头文件目录
include_directories(include)

# 源文件
file(GLOB SOURCES "src/*.cpp" "*.cpp")

# 创建可执行文件
add_executable(gemm ${SOURCES})
# 编译选项
#-O3 -ffast-math -march=native -mtune=native
target_compile_options(gemm PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>: -O3 -ffast-math -march=native -mtune=native -Wall -g -Wextra>
)
target_link_libraries(gemm PRIVATE OpenMP::OpenMP_CXX
)

