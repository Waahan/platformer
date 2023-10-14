#include "multiLib/misc.h"

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
}

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

} //multiLib
