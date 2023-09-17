#pragma once

#include <string>
#include <tuple>

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
}

#include "Estd.h"

namespace multiLib
{
    enum class initSystems : int { SDL = 0, Image, TTF, Mixer};

    class init
    {
        public:
        static init& getInit();

        init& initSystem(initSystems system);
        bool calledInit(initSystems system);

        private:
        init() = default;

        init(const init& copyFrom) = delete;
        init& operator=(const init& From) = delete;

        init(init&& moveFrom) = delete;
        init& operator=(init&& moveFrom) = delete;

        ~init();

        bool inits[4]{false};
    };

    class drawing
    {
        public:
        drawing() = default;

        drawing(const drawing& copyFrom) = delete;
        drawing& operator=(const drawing& copyFrom) = delete;

        drawing(drawing&& moveFrom) = delete;
        drawing& operator=(drawing&& moveFrom) = delete;

        virtual ~drawing() = default;

        virtual SDL_Texture* src() const =0;
        virtual const SDL_Rect& srcImage() const =0;
    };

    class renderWindow
    {
        public:
        renderWindow(const std::string& title, int width, int height);

        renderWindow(const renderWindow& copyFrom) = delete;
        renderWindow& operator=(const renderWindow& copyFrom) = delete;

        renderWindow(renderWindow&& moveFrom) = default;
        renderWindow& operator=(renderWindow&& moveFrom) = default;

        ~renderWindow() = default;

        std::tuple<int, int> realWindowSize();
        std::tuple<int, int> logicalWindowSize();

        void setIcon(const std::string& path);

        SDL_Texture* getImage(const std::string& path);
        SDL_Texture* convertSurfaceToTexture(SDL_Surface* convert);

        void clear();

        renderWindow& draw(const drawing& drawable, int destX, int destY);

        void display() const;

        private:
        Estd::custom_unique_ptr<SDL_Window, SDL_DestroyWindow> window;
        Estd::custom_unique_ptr<SDL_Renderer, SDL_DestroyRenderer> renderer;

        int logicalWidth, logicalHeight;
    };

} //multiLib
