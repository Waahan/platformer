cmake_minimum_required(VERSION 3.2...3.27)

project(Platformer VERSION 0.0.0.0 DESCRIPTION "A simple platforming game" HOMEPAGE_URL https://github.com/Waahan/platformer LANGUAGES CXX)

set(WARNING_FLAGS "-Wall;-Wextra;-Wpedantic" CACHE STRING "Warning flags for C++" FORCE)

find_package(SDL2 REQUIRED)
find_package(EnTT REQUIRED)
find_package(SDL2_image REQUIRED)

add_library(log STATIC src/core/log.h src/core/log.cpp)

target_compile_options(log PRIVATE ${WARNING_FLAGS})

add_executable(platformer WIN32 MACOSX_BUNDLE src/main.cpp)

target_include_directories(platformer PRIVATE src/core)

target_link_libraries(platformer PRIVATE SDL2 SDL2_image EnTT::EnTT log)
