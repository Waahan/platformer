cmake_minimum_required(VERSION 3.2...3.27)

project(platformer VERSION 1.1 DESCRIPTION "Base of project" HOMEPAGE_URL "https://www.github.com/Waahan/platformer" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

option(BUILD_RUNTIME_ERRORS "Build runtime error checking and logging" ON)

add_library(errors STATIC include/errors.h)

set_target_properties(errors
    PROPERTIES LINKER_LANGUAGE CXX
)

if(BUILD_RUNTIME_ERRORS)
    target_compile_definitions(errors PRIVATE BUILD_RUNTIME_ERRORS=1)
else()
    target_compile_definitions(errors PRIVATE BUILD_RUNTIME_ERRORS=0)
endif()

#Find SDL2 libraries
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(SDL_mixer REQUIRED)

add_library(multiLib STATIC include/multiLib/audio.h include/multiLib/events.h include/multiLib/misc.h include/multiLib/visuals.h src/multiLib/audio.cpp src/multiLib/events.cpp src/multiLib/misc.cpp src/multiLib/visuals.cpp) 
target_include_directories(multiLib PUBLIC include)
target_link_libraries(multiLib PUBLIC ${SDL2_LIBRARIES} ${SDL_IMAGE_LIB} ${SDL_TTF_LIB} ${SDL_MIX_LIB} errors)

add_library(networking STATIC include/networking.h src/networking.cpp)
target_include_directories(networking PUBLIC include)
target_link_libraries(networking PUBLIC errors)

#CPack 
set(CPACK_PACKAGE_NAME "platformer")
set(CPACK_PACKAGE_VENDOR "Waahan")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 1)
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

add_executable(main src/main.cpp) 

install(TARGETS main RUNTIME DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)

target_link_libraries(main PRIVATE multiLib networking)
