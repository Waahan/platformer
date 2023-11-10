#pragma once

#include <string>
#include <tuple>
#include <vector>

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
}

#include "Estd.h"
#include "misc.h"

namespace multiLib
{
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
        virtual const rectangle& srcImage() const =0;
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

        std::tuple<int, int> getSize() const;
        std::string getTitle() const;
        float getOpacity() const;

        void hideWindow();
        void showWindow();
        void fullScreenWindow();

        renderWindow& setSize(int width, int height);
        renderWindow& setIcon(const std::string& path);
        renderWindow& setTitle(const std::string& title);
        renderWindow& setOpacity(float opacity);

        SDL_Texture* getImage(const std::string& path);
        SDL_Texture* convertSurfaceToTexture(SDL_Surface* convert);

        void clear();

        renderWindow& draw(const drawing& drawable, int destX, int destY, int destWidth = 0, int destHeight = 0);

        void display() const;

        private:
        void createWindow(const std::string& title);
        void createRenderer();

        Estd::custom_unique_ptr<SDL_Window, SDL_DestroyWindow> window;
        Estd::custom_unique_ptr<SDL_Renderer, SDL_DestroyRenderer> renderer;

        int logicalWidth, logicalHeight;
    };

    class image : public drawing
    {
        public:
        image() = default;
        image(const std::string& path, rectangle rect, renderWindow& window);

        image(const image& copyFrom) = delete;
        image& operator=(const image& copyFrom) = delete;

        image(image&& moveFrom) = default;
        image& operator=(image&& moveFrom) = default;

        ~image() = default;

        SDL_Texture* src() const override { return texture.get(); }
        const rectangle& srcImage() const override { return images[index]; }

        image& operator++(int);
        image& addFrame(rectangle newRect);

        private:
        Estd::custom_unique_ptr<SDL_Texture, SDL_DestroyTexture> texture;
        std::vector<rectangle> images;
        int index;
        renderWindow& imageWindow;
    };

    enum class colours : int { black = 0, white, red, orange, yellow, green, blue, indigo, violet };
    SDL_Colour convertColour(colours colour);

    enum class fontStyles : int { normal = TTF_STYLE_NORMAL, bold = TTF_STYLE_BOLD, italic = TTF_STYLE_ITALIC, underline = TTF_STYLE_UNDERLINE, strikeThrought = TTF_STYLE_STRIKETHROUGH };

    class font
    {
        public:
        explicit font(std::string&& fontPath, int fontSize);

        font(const font& moveFrom) = delete;
        font& operator=(const font& From) = delete;

        font(font&& moveFrom) = default;
        font& operator=(font&& moveFrom) = default;

        ~font() = default;

        TTF_Font* getFont() const { return sdlFont.get(); }

        fontStyles getStyle() const;

        font& setStyle(fontStyles style);

        private:
        Estd::custom_unique_ptr<TTF_Font, TTF_CloseFont> sdlFont;
    };

    class message : public drawing
    {
        public:
        message() = default;
        message(std::string&& setMessage, font&& setFont, rectangle rect, colours setColour, renderWindow& setWindow);

        message(const message& copyFrom) = delete;
        message& operator=(const message& copyFrom) = delete;

        message(message&& moveFrom) = default;
        message& operator=(message&& moveFrom) = default;

        ~message() = default;

        SDL_Texture* src() const override { return texture.get(); }
        const rectangle& srcImage() const override { return dimensions; }

        message& setColour(colours newColour);
        message& setPos(int x, int y);
        message& setDimensions(int width, int height);
        message& setMessage(const std::string& message);

        private:
        void updateTexture();

        std::string messageString;
        colours colour;
        renderWindow& window;

        Estd::custom_unique_ptr<SDL_Texture, SDL_DestroyTexture> texture;
        rectangle dimensions;

        font messageFont;
    };
} //multiLib
