#FindSDL_mixer.cmake
#
# Finds SDL_MIX_LIB

find_library(
    SDL_MIX_LIB

    NAMES SDL_mixer libsdl2-mixer-2.0-0 libSDL2_mixer-2.0.so.0 libSDL2_mixer-2.0.so.0.2.2

    PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64

    DOC "SDL_mixer library path"

    REQUIRED
)
