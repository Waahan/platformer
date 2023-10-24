#include "multiLib/audio.h"

#include "errors.h"

namespace multiLib
{
    music::music(const std::string& path)
        : currentMusic{Mix_LoadMUS(path.c_str())}
    {
    /*
        construct a music object

        Precondition music path is valid
        Precondition music is FLAC, MP3, Ogg, VOC, WAV, MIDI, MOD, Opus audio format
    */
        runtime_assert(currentMusic, Mix_GetError());
    }

    bool music::paused() const
    {
    /*
        Return if the music channel is paused
    */
        return Mix_PausedMusic();
    }

    bool music::playing() const 
    {
    /*
        Return if the music channel is playing
    */
        return Mix_PlayingMusic();
    }

    bool music::fading() const
    {
    /*
        Return if the music channel is fading
    */
        Mix_Fading isFading = Mix_FadingMusic();

        return (isFading != MIX_NO_FADING);
    }

    music& music::setVolume(int setVolume)
    {
    /*
        Set the volume of the music channel

        Precondition setVolume must be between 0 and MIX_MAX_VOLUME
        Precondition music is not fading
    */
        debug_assert((setVolume > 0 && setVolume < MIX_MAX_VOLUME), "music volume must be between 0 and MIX_MAX_VOLUME");
        debug_assert(!(fading()), "setVolume should not be called while music is fading");

        Mix_VolumeMusic(setVolume);

        return *this;
    }

    music& music::fadeIn(std::chrono::milliseconds fadeInFor, int loops)
    {
    /*
        Fade the currentMusic in 

        Postcondition Mix_FadeInMusic returns 0 
    */
        bool hasError = Mix_FadeInMusic(currentMusic.get(), loops, (int)fadeInFor.count()); 

        runtime_assert(!hasError, Mix_GetError());

        return *this;
    }

    music& music::play(int loops)
    {
    /*
        Play currentMusic

        Precondition if music is fading call stop

        Postcondition Mix_PlayMusic returns 0
    */
        if(fading())
            stop();

        bool hasError = Mix_PlayMusic(currentMusic.get(), loops);

        runtime_assert(!hasError, Mix_GetError());

        return *this;
    }

    music& music::pause()
    {
    /*
        Pause the music channel
    */
        Mix_PauseMusic();

        return *this;
    }

    music& music::resume()
    {
    /*
        Resume the music channel
    */
        Mix_ResumeMusic();

        return *this;
    }

    music& music::stop()
    {
    /*
        Stop the music channel
    */
        Mix_HaltMusic();

        return *this;
    }

    music& music::fadeOut(std::chrono::milliseconds fadeOutFor)
    {
    /*
        Fade out music channel
    */
        Mix_FadeOutMusic((int)fadeOutFor.count());

        return *this;
    }

    sound::sound(const std::string& path)
        : currentChunk{Mix_LoadWAV(path.c_str())}, channel{-1}
    {
    /*
        Create a sound object

        Precondition path is a valid path
        Precondition path is FLAC, MP3, Ogg, VOC, WAV, MIDI, MOD, Opus audio format
    */
        runtime_assert(currentChunk, Mix_GetError());
    }

    bool sound::paused() const 
    {
    /*
        Return if sound is playing
    */
        if(ownsChannel())
            return Mix_Paused(channel);

        return false;
    }

    bool sound::playing() const
    {
    /*
        Return if sound is playing
    */
        if(ownsChannel())
            return Mix_Playing(channel); 

        return false;
    }

    sound& sound::setVolume(int setVolume)
    {
    /*
        Set the volume of sound

        Precondition setVolume greater than zero less than MIX_MAX_VOLUME
        
        Postcondition Mix_Volume chunk should not return -1
    */
        debug_assert((setVolume > 0 && setVolume < MIX_MAX_VOLUME), "sound volume must be between 0 and MIX_MAX_VOLUME");

        int hasError = Mix_VolumeChunk(currentChunk.get(), setVolume);

        debug_assert((hasError != -1), "sound chunk is so how NULL");

        return *this;
    }

    sound& sound::play(int loops)
    {
    /*
        Play sound and get it a channel

        Postcondition channel should not equal -1
    */
        channel = Mix_PlayChannel(channel, currentChunk.get(), loops); 

        runtime_assert((channel != -1), SDL_GetError()); 

        return *this;
    }

    sound& sound::pause()
    {
    /*
        Pause sound 
    */
        if(ownsChannel())
            Mix_Pause(channel);

        return *this;
    }

    sound& sound::resume()
    {
    /*
        Resume sound
    */
        if(ownsChannel())
            Mix_Resume(channel);

        return *this;
    }

    sound& sound::stop() 
    {
    /*
        Stop sound
    */
        if(ownsChannel())
            Mix_HaltChannel(channel);

        return *this;
    }

    bool sound::ownsChannel() const
    {
    /*
        Make sure sound owns channel
    */
        return (channel < 0) ? false : (currentChunk.get() == Mix_GetChunk(channel));
    }
} //multiLib
