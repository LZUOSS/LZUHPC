cmake_minimum_required(VERSION 3.10)
project(SPMM CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 设置CUDA标准
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 查找OpenMP包
find_package(OpenMP REQUIRED)
# 查找CUDA包
find_package(CUDAToolkit REQUIRED)

# 包含头文件目录
include_directories(include)

# 源文件
file(GLOB SOURCES "src/*.cpp" "src/*.cu" "*.cpp")

# 创建可执行文件
add_executable(spmm ${SOURCES})

# 编译选项
#-O3 -ffast-math -march=native -mtune=native
target_compile_options(spmm PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>: -O3 -ffast-math -march=native -mtune=native -Wall -g -Wextra>
    $<$<COMPILE_LANGUAGE:CUDA>:
    -Xcompiler -fopenmp
    -Xcompiler -O3
    -Xcompiler -ffast-math
    -Xcompiler -march=native
    -Xcompiler -Wall
    -O3 --use_fast_math>
)
# target_link_libraries(spmm PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(spmm PRIVATE OpenMP::OpenMP_CXX
     ${CUDA_LIBRARIES}
     PRIVATE CUDA::cusparse
    )

set_property(TARGET spmm PROPERTY CUDA_ARCHITECTURES 80)  # 根据你的GPU调整
