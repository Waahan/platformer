#include "multiLib.h"

#include <string>
#include <tuple>
#include <vector>
#include <chrono>

#include <cassert>

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

    inline bool init::calledInit(initSystems system)
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

    inline std::tuple<int, int> renderWindow::logicalWindowSize()
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

    message::message(const std::string& setMessage, const std::string& fontPath, int x, int y, int width, int height, colours setColour, renderWindow& setWindow)
        : messageString{std::move(setMessage)}, window{setWindow}, texture{}, dimensions{x, y, width, height}, font{TTF_OpenFont(fontPath.c_str(), 24)}
    {
    /*
        Precondition width and height are greater than zero
    */
        colour = setColour;

        assert((font) && TTF_GetError());

        updateTexture();
    }

    message& message::newColour(colours newColour)
    {
        colour = newColour;

        updateTexture();

        return *this;
    }

    message& message::newPos(int x, int y)
    {
        dimensions.x = x;
        dimensions.y = y;

        return *this;
    }

    message& message::newDimensions(int width, int height)
    {
    /*
        Precondition width and height are greater than zero
    */
        assert((width > 0 && height > 0) && "message width and heigth must be greater than zero");

        dimensions.w = width;
        dimensions.h = height;

        return *this;
    }

    message& message::newMessage(const std::string& message)
    {
        messageString = message;

        updateTexture();

        return *this;
    }

    message& message::newFont(const std::string& path)
    {
        font.reset(TTF_OpenFont( path.c_str(), fontSize) );

        assert((font) && TTF_GetError() );

        updateTexture();

        return *this;
    }

    message& message::newStyle(fontStyles style)
    {
        TTF_SetFontStyle( font.get(), (int)style);

        updateTexture();

        return *this;
    }

    inline void message::updateTexture()
    {
        Estd::custom_unique_ptr<SDL_Surface, SDL_FreeSurface> surface{ TTF_RenderText_Solid(font.get(), messageString.c_str(), convertColour(colour)) };

        assert((surface) && "Failed to render text");

        texture.reset( window.convertSurfaceToTexture(surface.get()) );
    }

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
        return (channel < 0) ? false : currentChunk.get() == Mix_GetChunk(channel);
    }

    bool keyboardInput::useEvent(const SDL_Event& event)
    {
    /*
        return true if event.type is a keyboard event
    */
        switch(event.type)
        {
            case SDL_KEYDOWN:
                handleEvent(event.key, true);
                return true;

            case SDL_KEYUP:
                handleEvent(event.key, false);
                return true;
        }
        return false;
    }

    bool keyboardInput::requestStatus(int inputButton) const
    {
    /*
        Use the int value of keyboardKeys as input button

        Precondition inputButton is a value of keyboardKeys
    */
        return keyboardStatus[inputButton];
    }

    void keyboardInput::handleEvent(const SDL_KeyboardEvent& event, bool upOrDown)
    {
    /*
        Precondtion in array bounds
    */
        keyboardStatus[event.keysym.scancode] = upOrDown;
    }

    eventHandler& eventHandler::get()
    {
        static eventHandler instance{};

        return instance; 
    }

    eventHandler& eventHandler::addInputDevice(inputDevice* device)
    {
        assert(device && "inputDevice must not be nullptr");

        devices.push_back(device);

        return *this;
    }

    void eventHandler::pollEvents()
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                exit(0);

            for(const auto& device : devices)
                if(device->useEvent(event))
                    break;
        }
    }

} //multiLib
