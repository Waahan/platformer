#include "multiLib/misc.h"

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
}

#include <cassert>

#include "multiLib/errors.h"

namespace multiLib
{
    init& init::getInit()
    {
        static init instance;

        return instance;
    }

    init& init::initSystem(initSystems system)
    {
        bool initResult = false;

        switch(system)
        {
            case initSystems::SDL:
                {
                    initResult = SDL_Init(SDL_INIT_EVERYTHING);
                    runtimeAssert(!initResult, SDL_GetError());
                }
                break;
            case initSystems::Image:
                {
                    int imgFlags = IMG_INIT_PNG;
                    int imgResult = 0;

                    imgResult = IMG_Init(imgFlags);
                    runtimeAssert(imgResult == imgFlags, SDL_GetError());
                }
                break;

            case initSystems::TTF:
                {
                    initResult = TTF_Init();
                    runtimeAssert(!initResult, TTF_GetError());
                }
                break;

            case initSystems::Mixer:
                {
                    int mixFlags = MIX_INIT_MP3;
                    int mixResult = 0;

                    mixResult = Mix_Init(mixFlags);
                    runtimeAssert(mixResult == mixFlags, Mix_GetError());
                    
                    int frequency = 44100;
                    unsigned short format = MIX_DEFAULT_FORMAT;
                    int channels = 2;
                    int chunksize = 2048;
                    initResult = Mix_OpenAudio(frequency, format, channels, chunksize);

                    runtimeAssert(!initResult, Mix_GetError());
                }
                break;
        }

        inits[(int)system] = true;

        return *this;
    }

    inline bool init::calledInit(initSystems system)
    {
        return inits[(int)system];
    }

    init::~init()
    {
        if(inits[(int)initSystems::SDL])
            SDL_Quit();

        if(inits[(int)initSystems::Image])
           IMG_Quit();

        if(inits[(int)initSystems::TTF])
            TTF_Quit();

        if(inits[(int)initSystems::Mixer])
        {
            Mix_Quit();
            Mix_CloseAudio();
        }
    }

    rectangle::rectangle(int setX, int setY, int setWidth, int setHeight)
        : x{setX}, y{setY}, width{setWidth}, height{setHeight}
    {
        assert((width > 0 && height > 0) && "width and height must be greater than zero");
    }

    SDL_Rect rectangle::toRect() const
    {
        return SDL_Rect{x, y, width, height};
    }

    rectangle& rectangle::setX(int setX)
    {
        x = setX;

        return *this;
    }

    rectangle& rectangle::setY(int setY)
    {
        x = setY;

        return *this;
    }

    rectangle& rectangle::setWidth(int setWidth)
    {
        assert(setWidth > 0 && "Width must be greater than zero");

        width = setWidth;

        return *this;
    }

    rectangle& rectangle::setHeight(int setHeight)
    {
        assert(setHeight > 0 && "Width must be greater than zero");

        height = setHeight;

        return *this;
    }
} //multiLib
