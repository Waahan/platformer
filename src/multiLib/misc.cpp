#include "multiLib/misc.h"

#include <cassert>

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
}

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
                    assert(!(initResult) && SDL_GetError());
                }
                break;
            case initSystems::Image:
                {
                    int imgFlags = IMG_INIT_PNG;
                    int imgResult = 0;

                    imgResult = IMG_Init(imgFlags);
                    assert( imgResult == imgFlags && "IMG_Init failed");
                }
                break;

            case initSystems::TTF:
                {
                    initResult = TTF_Init();
                    assert(!(initResult) && TTF_GetError());
                }
                break;

            case initSystems::Mixer:
                {
                    int mixFlags = MIX_INIT_MP3;
                    int mixResult = 0;

                    mixResult = Mix_Init(mixFlags);
                    assert(mixResult == mixFlags && "Mix_Init failed");
                    
                    int frequency = 44100;
                    unsigned short format = MIX_DEFAULT_FORMAT;
                    int channels = 2;
                    int chunksize = 2048;
                    initResult = Mix_OpenAudio(frequency, format, channels, chunksize);

                    assert(!(initResult) && "Mix_OpenAudio failed");
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

} //multiLib
