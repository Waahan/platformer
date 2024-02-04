#include "multiLib/misc.h"

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
}

#include "errors.h"

namespace multiLib
{
    Estd::initGuard<std::function<void(void)>> initSDL()
    {
    /*
        Init SDL2
        
        Postcondition SDL_Init returns 0
    */
        [[maybe_unused]] const bool sdlInitResult = SDL_Init(SDL_INIT_EVERYTHING);

        runtime_assert(!sdlInitResult, SDL_GetError());

        return Estd::initGuard<std::function<void(void)>>{SDL_Quit};
    }

    Estd::initGuard<std::function<void(void)>> initImage()
    { 
    /*
        Init SDL_Image

        Postcondition image init result equals image flags
    */
        int imageInitFlags = IMG_INIT_PNG;

        [[maybe_unused]] const int imageInitResult = IMG_Init(imageInitFlags);

        runtime_assert((imageInitResult == imageInitFlags), SDL_GetError());

        return Estd::initGuard<std::function<void(void)>>{IMG_Quit};
    }

    Estd::initGuard<std::function<void(void)>> initTTF()
    {
    /*
        Init SDL_TTF 

        Postcondition TTF_Init returns 0
    */
        [[maybe_unused]] const bool ttfInitResult = TTF_Init();

        runtime_assert(!ttfInitResult, TTF_GetError());

        return Estd::initGuard<std::function<void(void)>>{TTF_Quit};
    }

    Estd::initGuard<std::function<void(void)>> initMixer()
    {
    /*
        Init SDL_Mixer

        Postcondition Mix_Init returns mixFlags
    */
        const int mixFlags = MIX_INIT_MP3;

        [[maybe_unused]] const int mixResult = Mix_Init(mixFlags);

        runtime_assert((mixResult == mixFlags), Mix_GetError());
        
        return Estd::initGuard<std::function<void(void)>>{Mix_Quit};
    }

    Estd::initGuard<std::function<void(void)>> openAudio()
    {
    /*
        Open SDL audio

        Postcondition Mix_OpenAudio returns 0
    */
        const int frequency = 44100;
        const unsigned short format = MIX_DEFAULT_FORMAT;
        const int channels = 2;
        const int chunksize = 2048;

        [[maybe_unused]] const bool initResult = Mix_OpenAudio(frequency, format, channels, chunksize);

        runtime_assert(!initResult, Mix_GetError());

        return Estd::initGuard<std::function<void(void)>>{Mix_CloseAudio};  
    }

    rectangle::rectangle(int setX, int setY, int setWidth, int setHeight)
        : x{setX}, y{setY}, width{setWidth}, height{setHeight}
    {
    /*
        Construct a rectangle

        Precondition width and height are greater than zero
    */
        debug_assert((width > 0 && height > 0), "width and height must be greater than zero");
    }

    SDL_Rect rectangle::toRect() const
    {
    /*
        Return the rectangles SDL representation
    */
        return SDL_Rect{x, y, width, height};
    }

    rectangle& rectangle::setX(int setX)
    {
    /*
        Set the x for the rectangle
    */
        x = setX;

        return *this;
    }

    rectangle& rectangle::setY(int setY)
    {
    /*
        Set the y position for the rectangle
    */
        y = setY;

        return *this;
    }

    rectangle& rectangle::setWidth(int setWidth)
    {
    /*
        Set the width of the rectangle

        Precondition setWidth is greater than zero
    */
        debug_assert((setWidth > 0), "Width must be greater than zero");

        width = setWidth;

        return *this;
    }

    rectangle& rectangle::setHeight(int setHeight)
    {
    /*
        Set the height for the rectangle

        Precondition setHeight is greater than zero
    */
        debug_assert((setHeight > 0), "Width must be greater than zero");

        height = setHeight;

        return *this;
    }
} //multiLib
