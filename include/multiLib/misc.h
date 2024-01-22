#pragma once

#include <functional>

extern "C"
{
    #include <SDL2/SDL.h>
}

#include "Estd.h"

namespace multiLib
{
    Estd::initGuard<std::function<void(void)>> initSDL();
    Estd::initGuard<std::function<void(void)>> initImage();
    Estd::initGuard<std::function<void(void)>> initTTF();
    Estd::initGuard<std::function<void(void)>> initMixer();
    Estd::initGuard<std::function<void(void)>> openAudio();

    class rectangle
    {
        public:
        rectangle() = default;
        rectangle(int setX, int setY, int setWidth, int setHeight);

        rectangle(const rectangle& copyFrom) = default;
        rectangle& operator=(const rectangle& copyFrom) = default;

        rectangle(rectangle&& moveFrom) = default;
        rectangle& operator=(rectangle&& moveFrom) = default;

        ~rectangle() = default;

        int getX() const { return x; } 
        int getY() const { return y; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }

        SDL_Rect toRect() const;

        rectangle& setX(int setX);
        rectangle& setY(int setY);
        rectangle& setWidth(int setWidth);
        rectangle& setHeight(int setHeight);

        private:
        int x, y, width, height;
    };
} // multiLib
