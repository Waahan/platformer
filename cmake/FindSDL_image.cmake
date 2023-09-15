#FindSDL_image.cmake
#
# Finds SDL_IMAGE_LIB

find_library(
    SDL_IMAGE_LIB

    NAMES SDL_image libsdl2-image-2.0-0 libSDL2_image-2.0.so.0 libSDL2_image-2.0.so.0.2.3

    PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64

    DOC "SDL_image library path"

    REQUIRED
)
