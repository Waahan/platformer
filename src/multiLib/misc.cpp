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
    init& init::getInit()
    {
    /*
        Create and instance of init once
    */
        static init instance;

        return instance;
    }

    init& init::initSystem(initSystems system)
    {
    /*
        Init a system
    */
        bool initResult = false;

        switch(system)
        {
            case initSystems::SDL:
                {
                    initResult = SDL_Init(SDL_INIT_EVERYTHING);
                    runtime_assert(!initResult, SDL_GetError());
                }
                break;
            case initSystems::Image:
                {
                    int imgFlags = IMG_INIT_PNG;
                    int imgResult = 0;

                    imgResult = IMG_Init(imgFlags);
                    runtime_assert((imgResult == imgFlags), SDL_GetError());
                }
                break;

            case initSystems::TTF:
                {
                    initResult = TTF_Init();
                    runtime_assert(!initResult, TTF_GetError());
                }
                break;

            case initSystems::Mixer:
                {
                    int mixFlags = MIX_INIT_MP3;
                    int mixResult = 0;

                    mixResult = Mix_Init(mixFlags);
                    runtime_assert((mixResult == mixFlags), Mix_GetError());
                    
                    int frequency = 44100;
                    unsigned short format = MIX_DEFAULT_FORMAT;
                    int channels = 2;
                    int chunksize = 2048;
                    initResult = Mix_OpenAudio(frequency, format, channels, chunksize);

                    runtime_assert(!initResult, Mix_GetError());
                }
                break;
        }

        inits[(int)system] = true;

        return *this;
    }

    inline bool init::calledInit(initSystems system)
    {
    /*
        Check if a system was inited
    */
        return inits[(int)system];
    }

    init::~init()
    {
    /*
        Call exit init functions
    */
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
