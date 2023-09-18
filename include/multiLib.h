#pragma once

#include <string>
#include <tuple>
#include <vector>

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

    class image : public drawing
    {
        public:
        image(const std::string& path, int x, int y, int width, int height, renderWindow& window);

        image(const image& copyFrom) = delete;
        image& operator=(const image& copyFrom) = delete;

        image(image&& moveFrom) = default;
        image& operator=(image&& moveFrom) = default;

        ~image() = default;

        SDL_Texture* src() const override { return texture.get(); }
        const SDL_Rect& srcImage() const override { return images[index]; }

        image& operator++(int);
        image& addFrame(int x, int y, int width, int height);

        private:
        Estd::custom_unique_ptr<SDL_Texture, SDL_DestroyTexture> texture;
        std::vector<SDL_Rect> images;
        int index;
        renderWindow& imageWindow;
    };

    enum class colours : int { black = 0, white, red, orange, yellow, green, blue, indigo, violet };
    SDL_Colour convertColour(colours colour);

    enum class fontStyles : int { normal = TTF_STYLE_NORMAL, bold = TTF_STYLE_BOLD, italic = TTF_STYLE_ITALIC, underline = TTF_STYLE_UNDERLINE, strikeThrought = TTF_STYLE_STRIKETHROUGH };

    class message : public drawing
    {
        public:
        message(const std::string& setMessage, const std::string& fontPath, int x, int y, int width, int height, colours setColour, renderWindow& setWindow);

        message(const message& copyFrom) = delete;
        message& operator=(const message& copyFrom) = delete;

        message(message&& moveFrom) = default;
        message& operator=(message&& moveFrom) = default;

        ~message() = default;

        SDL_Texture* src() const override { return texture.get(); }
        const SDL_Rect& srcImage() const override { return dimensions; }

        message& newColour(colours newColour);
        message& newPos(int x, int y);
        message& newDimensions(int width, int height);
        message& newMessage(const std::string& message);

        message& newFont(const std::string& fontPath);
        message& newStyle(fontStyles style);

        private:
        void updateTexture();

        std::string messageString;
        colours colour;
        renderWindow& window;

        Estd::custom_unique_ptr<SDL_Texture, SDL_DestroyTexture> texture;
        SDL_Rect dimensions;

        Estd::custom_unique_ptr<TTF_Font, TTF_CloseFont> font;
        int fontSize = 36;
    };
} //multiLib
