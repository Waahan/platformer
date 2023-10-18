#include "multiLib/visuals.h"

#include <cassert>

#include "multiLib/errors.h"

namespace multiLib
{
    renderWindow::renderWindow(const std::string& title, int width, int height)
        : window{}, renderer{}, logicalWidth{width}, logicalHeight{height}
    {
    /*
        Precondition width and height are greater than zero
    */
        assert( ( width > 0 && height > 0 ) && "window width and height must be greater than zero" );
        
        window.reset( SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE) );

        runtimeAssert(window, SDL_GetError());

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

        renderer.reset( SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED) );

        runtimeAssert(renderer, SDL_GetError());

        bool hasError = SDL_RenderSetLogicalSize(renderer.get(), width, height);

        runtimeAssert(!hasError, SDL_GetError());
    }

    std::tuple<int, int> renderWindow::getSize() const
    {
        return std::make_tuple(logicalWidth, logicalHeight);
    }

    std::string renderWindow::getTitle() const
    {
        return SDL_GetWindowTitle(window.get());
    }

    float renderWindow::getOpacity() const
    {
        float opacity = 0;

        bool hasError = SDL_GetWindowOpacity(window.get(), &opacity);

        runtimeAssert(!hasError, SDL_GetError());

        return opacity;
    }

    void renderWindow::requestAttention(flash annoyannce) const
    {
        bool hasError = SDL_FlashWindow(window.get(), (SDL_FlashOperation)annoyannce);

        runtimeAssert(!hasError, SDL_GetError());
    }

    void renderWindow::hideWindow()
    {
        SDL_HideWindow(window.get());
    }
    
    void renderWindow::showWindow()
    {
        SDL_ShowWindow(window.get());
    }

    void renderWindow::fullScreenWindow()
    {
        bool hasError = SDL_SetWindowFullscreen(window.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);

        runtimeAssert(!hasError, SDL_GetError());
    }

    renderWindow& renderWindow::setSize(int width, int height)
    {
    /*
        Precondition width and height are greater than zero
    */
        assert((width > 0 && height > 0) && "window width and height must be greater than zero");

        SDL_SetWindowSize(window.get(), width, height);

        return *this;
    }

    renderWindow& renderWindow::setLogicalSize(int width, int height)
    {
    /*
        Precondition width and height are greater than zero
    */
        assert((width > 0 && height > 0) && "window width and height must be greater than zero");

        bool hasError = SDL_RenderSetLogicalSize(renderer.get(), width, height);

        runtimeAssert(!hasError, SDL_GetError());

        logicalWidth = width;
        logicalHeight = height;

        return *this;
    }

    renderWindow& renderWindow::setIcon(const std::string& path)
    {
        Estd::custom_unique_ptr<SDL_Surface, SDL_FreeSurface> icon{ IMG_Load(path.c_str()) };

        runtimeAssert(icon, SDL_GetError());

        SDL_SetWindowIcon(window.get(), icon.get() );

        return *this;
    }

    renderWindow& renderWindow::setTitle(const std::string& title)
    {
        SDL_SetWindowTitle(window.get(), title.c_str());

        return *this;
    }

    renderWindow& renderWindow::setOpacity(float opacity)
    {
    /*
        Precondition opacity must be 0.0f or greater and 1.0f or less
    */
        assert((opacity >= 0.0f && opacity <= 1.0f) && "opacity must be between 0.0f and 1.0f");

        bool hasError = SDL_SetWindowOpacity(window.get(), opacity);

        runtimeAssert(!hasError, SDL_GetError());

        return *this;
    }

    SDL_Texture* renderWindow::getImage(const std::string& path)
    {
        SDL_Texture* newImage = IMG_LoadTexture(renderer.get(), path.c_str());

        runtimeAssert(newImage, SDL_GetError());
        
        return newImage;
    }

    SDL_Texture* renderWindow::convertSurfaceToTexture(SDL_Surface* convert)
    {
    /*
        Precondition convert is not nullptr
    */
        assert( (convert) && "Can not convert a nullptr to a texture");

        SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer.get(), convert);

        runtimeAssert(newTexture, SDL_GetError());
    
        return newTexture;
    }

    void renderWindow::clear()
    {
        bool hasError = SDL_RenderClear(renderer.get());

        runtimeAssert(!hasError, SDL_GetError());
    }

    renderWindow& renderWindow::draw(const drawing& drawable, int destX, int destY)
    {
        SDL_Rect dest {destX, destY, drawable.srcImage().getWidth(), drawable.srcImage().getHeight()};

        SDL_Rect src {drawable.srcImage().toRect()};

        bool hasError = SDL_RenderCopy(renderer.get(), drawable.src(), &src, &dest);

        runtimeAssert(!hasError, SDL_GetError());

        return *this;
    }

    void renderWindow::display() const
    {
        SDL_RenderPresent(renderer.get());
    }

    image::image(const std::string& path, rectangle rect, renderWindow& window)
        : texture{window.getImage(path)}, images{}, index{}, imageWindow{window}
    {
        images.push_back(rect);
    }

    image& image::operator++(int)
    {
    /*
        Loop over images vector
    */
        (index < (int)images.size() - 1) ? index++ : index = 0; 

        return *this;
    }

    image& image::addFrame(rectangle newRect) 
    {
        images.push_back(newRect);

        return *this;
    }

    SDL_Colour convertColour(colours colour)
    {
        //The colours are using RGBA
        switch(colour)
        {
            case colours::black:
                return SDL_Colour{0, 0, 0, 1};

            case colours::white: 
                return SDL_Colour{255, 255, 255, 1};

            case colours::red:
                return SDL_Colour{255, 0, 0, 1};

            case colours::orange: 
                return SDL_Colour{255, 165, 0, 1};

            case colours::yellow:
                return SDL_Colour{255, 255, 0, 1};

            case colours::green:
                return SDL_Colour{0, 255, 0, 1};

            case colours::blue:
                return SDL_Colour{0, 0, 255, 1};

            case colours::indigo:
                return SDL_Colour{75, 0, 130, 1};

            case colours::violet:
                return SDL_Colour{238, 130, 238, 1};
        }

        return SDL_Colour{0, 0, 0, 1};
    }

    font::font(std::string&& fontPath, int fontSize)
        : sdlFont{TTF_OpenFont(fontPath.c_str(), fontSize)}
    {
    }

    message::message(std::string&& setMessage, font&& setFont, rectangle rect, colours setColour, renderWindow& setWindow)
        : messageString{std::move(setMessage)}, window{setWindow}, texture{}, dimensions{rect}, messageFont{std::move(setFont)}
    {
    /*
        Precondition width and height are greater than zero
    */
        colour = setColour;

        updateTexture();
    }

    message& message::setColour(colours newColour)
    {
        colour = newColour;

        updateTexture();

        return *this;
    }

    message& message::setPos(int x, int y)
    {
        dimensions.setX(x);
        dimensions.setY(y);

        return *this;
    }

    message& message::setDimensions(int width, int height)
    {
        dimensions.setWidth(width);
        dimensions.setHeight(height);

        return *this;
    }

    message& message::setMessage(const std::string& message)
    {
        messageString = message;

        updateTexture();

        return *this;
    }

    void message::updateTexture()
    {
        Estd::custom_unique_ptr<SDL_Surface, SDL_FreeSurface> surface{ TTF_RenderText_Solid(messageFont.getFont(), messageString.c_str(), convertColour(colour)) };

        runtimeAssert(surface, TTF_GetError());

        texture.reset( window.convertSurfaceToTexture(surface.get()) );
    }
} //multiLib
