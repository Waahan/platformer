cmake_minimum_required(VERSION 3.2)

project(platformer VERSION 0.1 DESCRIPTION "Base of project" LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL_image REQUIRED)

find_package(SDL_ttf REQUIRED)

find_package(SDL_mixer REQUIRED)

include_directories(include)
add_executable(main src/main.cpp src/multiLib.cpp)

target_link_libraries(main PUBLIC ${SDL2_LIBRARIES} ${SDL_IMAGE_LIB} ${SDL_TTF_LIB} ${SDL_MIX_LIB})
