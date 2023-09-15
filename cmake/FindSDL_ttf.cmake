#FindSDL_ttf.cmake
#
# Finds SDL_TTF_LIB 

find_library(
    SDL_TTF_LIB

    NAMES SDL_ttf libsdl2-ttf-2.0-0 libSDL2_ttf-2.0.so.0 libSDL2_ttf-2.0.so.0.14.1

    PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 

    DOC "SDL_ttf library path"

    REQUIRED
)
