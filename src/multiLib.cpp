#include <string>
#include <tuple>

#include <cassert>

extern "C"
{
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
}

#include "Estd.h"
#include "multiLib.h"

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
                    assert(!(initResult) && SDL_GetError());
                }
                break;
            case initSystems::Image:
                {
                    int imgFlags = IMG_INIT_PNG;
                    int imgResult = 0;

                    imgResult = IMG_Init(imgFlags);
                    assert( imgResult == imgFlags && "IMG_Init failed");
                }
                break;

            case initSystems::TTF:
                {
                    initResult = TTF_Init();
                    assert(!(initResult) && TTF_GetError());
                }
                break;

            case initSystems::Mixer:
                {
                    int mixFlags = MIX_INIT_MP3;
                    int mixResult = 0;

                    mixResult = Mix_Init(mixFlags);
                    assert(mixResult == mixFlags && "Mix_Init failed");
                    
                    int frequency = 44100;
                    unsigned short format = MIX_DEFAULT_FORMAT;
                    int channels = 2;
                    int chunksize = 2048;
                    initResult = Mix_OpenAudio(frequency, format, channels, chunksize);

                    assert(!(initResult) && "Mix_OpenAudio failed");
                }
                break;
        }

        inits[(int)system] = true;

        return *this;
    }

    bool init::calledInit(initSystems system)
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

    renderWindow::renderWindow(const std::string& title, int width, int height)
        : window{}, renderer{}, logicalWidth{width}, logicalHeight{height}
    {
    /*
        Precondition width and height are greater than zero
    */
        assert( ( width > 0 && height > 0 ) && "window width and height must be greater than zero" );
        
        window.reset( SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE) );

        assert( (window) && SDL_GetError() );

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

        renderer.reset( SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED) );

        assert( (renderer) && SDL_GetError() );

        bool hasError = SDL_RenderSetLogicalSize(renderer.get(), width, height);

        assert( !(hasError) && SDL_GetError() );
    }

    std::tuple<int, int> renderWindow::realWindowSize()
    {
        int width, height;

        SDL_GetWindowSize(window.get(), &width, &height);

        return std::make_tuple(width, height);
    }

    std::tuple<int, int> renderWindow::logicalWindowSize()
    {
        return std::make_tuple(logicalWidth, logicalHeight);
    }

    void renderWindow::setIcon(const std::string& path)
    {
        Estd::custom_unique_ptr<SDL_Surface, SDL_FreeSurface> icon{ IMG_Load(path.c_str()) };

        assert( (icon) && SDL_GetError() );

        SDL_SetWindowIcon(window.get(), icon.get() );
    }

    SDL_Texture* renderWindow::getImage(const std::string& path)
    {
        SDL_Texture* newImage = IMG_LoadTexture(renderer.get(), path.c_str());

        assert( (newImage) && "Failed to load image" );
        
        return newImage;
    }

    SDL_Texture* renderWindow::convertSurfaceToTexture(SDL_Surface* convert)
    {
        assert( (convert) && "Can not convert a nullptr to a texture");

        SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer.get(), convert);

        assert( (newTexture) && SDL_GetError() );
    
        return newTexture;
    }

    void renderWindow::clear()
    {
        bool hasError = SDL_RenderClear(renderer.get());

        assert( !(hasError) && SDL_GetError() );
    }

    renderWindow& renderWindow::draw(const drawing& drawable, int destX, int destY)
    {
        SDL_Rect dest {destX, destY, drawable.srcImage().w, drawable.srcImage().h};

        bool hasError = SDL_RenderCopy(renderer.get(), drawable.src(), &drawable.srcImage(), &dest);

        assert( !(hasError) && SDL_GetError() );

        return *this;
    }

    void renderWindow::display() const
    {
        SDL_RenderPresent(renderer.get());
    }

    image::image(const std::string& path, int x, int y, int width, int height, renderWindow& window)
        : texture{window.getImage(path)}, images{}, index{}, imageWindow{window}
    {
    /*
        Precondition width and height are greater than zero
    */
        assert( (width > 0 && height > 0) && "image width and height must be greater than zero");

        images.push_back(SDL_Rect{x, y, width, height});
    }

    image& image::operator++(int)
    {
    /*
        Loop over images vector
    */
        (index < (int)images.size() - 1) ? index++ : index = 0; 

        return *this;
    }

    image& image::addFrame(int x, int y, int width, int height)
    {
    /*
        Precondition width and height are greater than zero
    */
        assert( (width > 0 && height > 0) && "image width and height should be greater than zero");

        images.push_back(SDL_Rect{x, y, width, height});

        return *this;
    }
} //multiLib
