#pragma once

extern "C"
{
    #include <SDL2/SDL.h>
}

namespace multiLib
{
    enum class initSystems : int { SDL = 0, Image, TTF, Mixer};

    class init
    {
        public:
        static init& getInit();

        init& initSystem(initSystems system);
        inline bool calledInit(initSystems system);

        private:
        init() = default;

        init(const init& copyFrom) = delete;
        init& operator=(const init& From) = delete;

        init(init&& moveFrom) = delete;
        init& operator=(init&& moveFrom) = delete;

        ~init();

        bool inits[4]{false};
    };

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
