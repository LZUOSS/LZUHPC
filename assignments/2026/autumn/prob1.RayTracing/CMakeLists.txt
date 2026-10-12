cmake_minimum_required(VERSION 3.12)
project(RayTracing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# clangd reads the compilation database from build/.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep the toolchain's flags and explicit user configuration.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(RAYTRACING_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_executable(RayTracing
    src/main.cpp
    src/geometry/hittable_list.cpp
    src/geometry/sphere.cpp
    src/material/material.cpp
    src/math/color.cpp
    src/math/interval.cpp
    src/render/camera.cpp
)

target_include_directories(RayTracing PRIVATE src)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(RayTracing PRIVATE -Wall -Wextra -Wpedantic)
    if(RAYTRACING_WARNINGS_AS_ERRORS)
        target_compile_options(RayTracing PRIVATE -Werror)
    endif()
elseif(MSVC)
    target_compile_options(RayTracing PRIVATE /W4)
    if(RAYTRACING_WARNINGS_AS_ERRORS)
        target_compile_options(RayTracing PRIVATE /WX)
    endif()
endif()
