#include "multiLib/audio.h"

#include <cassert>

namespace multiLib
{
    music::music(const std::string& path)
        : currentMusic{Mix_LoadMUS(path.c_str())}
    {
        assert((currentMusic) && Mix_GetError());
    }

    bool music::paused() const
    {
        return Mix_PausedMusic();
    }

    bool music::playing() const 
    {
        return Mix_PlayingMusic();
    }

    bool music::fading() const
    {
        Mix_Fading isFading = Mix_FadingMusic();

        return (isFading != MIX_NO_FADING);
    }

    music& music::newVolume(int setVolume)
    {
    /*
        Precondition setVolume must be between 0 and MIX_MAX_VOLUME
        Precondition music is not fading
    */
        assert( (setVolume > 0 && setVolume < MIX_MAX_VOLUME) && "music volume must be between 0 and MIX_MAX_VOLUME");
        assert( !(fading()) && "newVolume should not be called while music is fading");

        Mix_VolumeMusic(setVolume);

        return *this;
    }

    music& music::fadeIn(std::chrono::milliseconds fadeInFor, int loops)
    {
        bool hasError = Mix_FadeInMusic(currentMusic.get(), loops, (int)fadeInFor.count()); 

        assert(!(hasError) && Mix_GetError());

        return *this;
    }

    music& music::play(int loops)
    {
    /*
        Precondition if music is fading call stop
    */
        if(fading())
            stop();

        bool hasError = Mix_PlayMusic(currentMusic.get(), loops);

        assert(!(hasError) && Mix_GetError());

        return *this;
    }

    music& music::pause()
    {
        Mix_PauseMusic();

        return *this;
    }

    music& music::resume()
    {
        Mix_ResumeMusic();

        return *this;
    }

    music& music::stop()
    {
        Mix_HaltMusic();

        return *this;
    }

    music& music::fadeOut(std::chrono::milliseconds fadeOutFor)
    {
        Mix_FadeOutMusic((int)fadeOutFor.count());

        return *this;
    }

    sound::sound(const std::string& path)
        : currentChunk{Mix_LoadWAV(path.c_str())}, channel{-1}
    {
        assert((currentChunk) && Mix_GetError());
    }

    bool sound::paused() const 
    {
        if(ownsChannel())
            return Mix_Paused(channel);

        return false;
    }

    bool sound::playing() const
    {
        if(ownsChannel())
            return Mix_Playing(channel); 

        return false;
    }

    sound& sound::newVolume(int setVolume)
    {
    /*
        Precondition setVolume greater than zero less than MIX_MAX_VOLUME
    */
        assert((setVolume > 0 && setVolume < MIX_MAX_VOLUME) && "sound volume must be between 0 and MIX_MAX_VOLUME");

        Mix_VolumeChunk(currentChunk.get(), setVolume);

        return *this;
    }

    sound& sound::play(int loops)
    {
        channel = Mix_PlayChannel(channel, currentChunk.get(), loops); 

        assert( (channel != -1) && "Chunk could not be played");

        return *this;
    }

    sound& sound::pause()
    {
        if(ownsChannel())
            Mix_Pause(channel);

        return *this;
    }

    sound& sound::resume()
    {
        if(ownsChannel())
            Mix_Resume(channel);

        return *this;
    }

    sound& sound::stop() 
    {
        if(ownsChannel())
            Mix_HaltChannel(channel);

        return *this;
    }

    inline bool sound::ownsChannel() const
    {
        return (channel < 0) ? false : (currentChunk.get() == Mix_GetChunk(channel));
    }

} //multiLib
