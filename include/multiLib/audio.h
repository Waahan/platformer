#pragma once

#include <string>
#include <chrono>

extern "C"
{
    #include <SDL2/SDL_mixer.h>
}

#include "Estd.h"

namespace multiLib
{
    class audio
    {
        public:
        audio() = default;

        audio(const audio& copyFrom) = delete; 
        audio& operator=(const audio& copyFrom) = delete; 

        audio(audio&& moveFrom) = delete; 
        audio& operator=(audio&& moveFrom) = delete; 

        virtual ~audio() = default;

        virtual bool paused() const =0;
        virtual bool playing() const =0;

        virtual audio& newVolume(int setVolume) =0;

        virtual audio& play(int loops = 0) =0;

        virtual audio& pause() =0;
        virtual audio& resume() =0;
        
        virtual audio& stop() =0;
    };

    class music : public audio
    {
        public:
        explicit music(const std::string& path);

        music(const music& copyFrom) = delete;
        music& operator=(const music& copyFrom) = delete;

        music(music&& moveFrom) = default;
        music& operator=(music&& moveFrom) = default;

        ~music() = default;

        bool paused() const override;
        bool playing() const override;
        bool fading() const;

        music& newVolume(int setVolume) override;

        music& fadeIn(std::chrono::milliseconds fadeInFor, int loops = 0);
        music& play(int loops = 0) override;

        music& pause() override;
        music& resume() override;

        music& stop() override;
        music& fadeOut(std::chrono::milliseconds fadeOutFor);

        private:
        Estd::custom_unique_ptr<Mix_Music, Mix_FreeMusic> currentMusic;
    };

    class sound : public audio
    {
        public:
        explicit sound(const std::string& path);
        
        sound(const sound& copyFrom) = delete;
        sound& operator=(const sound& copyFrom) = delete;

        sound(sound&& moveFrom) = default;
        sound& operator=(sound&& moveFrom) = default;
        
        ~sound() = default;

        bool paused() const override;
        bool playing() const override;

        sound& newVolume(int setVolume) override;

        sound& play(int loops = 0) override;

        sound& pause() override;
        sound& resume() override;

        sound& stop() override;

        private:
        inline bool ownsChannel() const;

        Estd::custom_unique_ptr<Mix_Chunk, Mix_FreeChunk> currentChunk;
        int channel;
    };
} //multiLib
