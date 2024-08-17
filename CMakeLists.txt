cmake_minimum_required(VERSION 3.2...3.27)

project(Platformer VERSION 0.0.0.0 DESCRIPTION "A simple platforming game" HOMEPAGE_URL https://github.com/Waahan/platformer LANGUAGES CXX)

#Essential packages
find_package(SDL2 REQUIRED)
find_package(EnTT REQUIRED)
find_package(SDL2_image REQUIRED)

#Game executable
add_executable(platformer WIN32 MACOSX_BUNDLE src/main.cpp)

target_link_libraries(platformer PRIVATE SDL2 SDL2_image EnTT::EnTT)
