cmake_minimum_required(VERSION 3.2...3.27)

project(platformer VERSION 0.1 DESCRIPTION "Base of project" HOMEPAGE_URL "https://www.github.com/Waahan/platformer" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

option(BUILD_RUNTIME_ERRORS "Build runtime error checking and logging" ON)

if(BUILD_RUNTIME_ERRORS)
    message(WARNING "building runtime error checking isn't currently working")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL_image REQUIRED)

find_package(SDL_ttf REQUIRED)

find_package(SDL_mixer REQUIRED)

#CPack 
set(CPACK_PACKAGE_NAME "platformer")
set(CPACK_PACKAGE_VENDOR "Waahan")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_PACKAGE_VERSION_MAJOR 0.1)
set(CPACK_PACKAGE_VERSION_MINOR 0.1)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_PACKAGE_DESCRIPTION "A platforming game about something I will come up with")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A platforming game")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.github.com/Waahan/platformer")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/assets/icon.png")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set(CPACK_OUTPUT_CONFIG_FILE CPackConfig.cmake)
set(CPACK_VERBATIM_VARIABLES FALSE)
include(CPack)

include_directories(include)
add_executable(main src/main.cpp src/multiLib/audio.cpp src/multiLib/events.cpp src/multiLib/misc.cpp src/multiLib/visuals.cpp src/errors.cpp src/networking.cpp)

install(TARGETS main RUNTIME DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)

target_link_libraries(main PUBLIC ${SDL2_LIBRARIES} ${SDL_IMAGE_LIB} ${SDL_TTF_LIB} ${SDL_MIX_LIB})
