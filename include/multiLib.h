#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <chrono>

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

    enum class flash : int { cancel = SDL_FLASH_CANCEL, brief = SDL_FLASH_BRIEFLY, untilUser = SDL_FLASH_UNTIL_FOCUSED};

    class renderWindow
    {
        public:
        renderWindow(const std::string& title, int width, int height);

        renderWindow(const renderWindow& copyFrom) = delete;
        renderWindow& operator=(const renderWindow& copyFrom) = delete;

        renderWindow(renderWindow&& moveFrom) = default;
        renderWindow& operator=(renderWindow&& moveFrom) = default;

        ~renderWindow() = default;

        std::tuple<int, int> realWindowSize() const;
        inline std::tuple<int, int> logicalWindowSize() const;
        std::string getTitle() const;
        float getOpacity() const;

        void requestAttention(flash annoyance) const;
        void hideWindow();
        void showWindow();
        void fullScreenWindow();

        renderWindow& setSize(int width, int height);
        renderWindow& setLogicalSize(int width, int height);
        renderWindow& setIcon(const std::string& path);
        renderWindow& setTitle(const std::string& title);
        renderWindow& setOpacity(float opacity);

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

        inline SDL_Texture* src() const override { return texture.get(); }
        inline const SDL_Rect& srcImage() const override { return images[index]; }

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
        inline void updateTexture();

        std::string messageString;
        colours colour;
        renderWindow& window;

        Estd::custom_unique_ptr<SDL_Texture, SDL_DestroyTexture> texture;
        SDL_Rect dimensions;

        Estd::custom_unique_ptr<TTF_Font, TTF_CloseFont> font;
        int fontSize = 36;
    };

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

    class inputDevice
    {
        public:
        inputDevice() = default;

        inputDevice(const inputDevice& copyFrom) = delete;
        inputDevice& operator=(const inputDevice& copyFrom) = delete;

        inputDevice(inputDevice&& moveFrom) = delete;
        inputDevice& operator=(inputDevice&& moveFrom) = delete;

        ~inputDevice() = default;
        
        virtual bool useEvent(const SDL_Event& event) =0;
        virtual bool requestStatus(int inputButton) const =0;
    };

    enum class keyboardKeys : int {w=SDL_SCANCODE_W, a=SDL_SCANCODE_A, s=SDL_SCANCODE_S, d=SDL_SCANCODE_D};

    class keyboardInput : public inputDevice
    {
        public:
        keyboardInput() = default;

        keyboardInput(const keyboardInput& copyFrom) = delete;
        keyboardInput& operator=(const keyboardInput& copyFrom) = delete;

        keyboardInput(keyboardInput&& moveFrom) = delete;
        keyboardInput& operator=(keyboardInput&& moveFrom) = delete;

        ~keyboardInput() = default;

        bool useEvent(const SDL_Event& event) override;
        bool requestStatus(int inputButton) const override;

        inline bool getKey(keyboardKeys key) const { return keyboardStatus[(int)key]; }

        private:
        void handleEvent(const SDL_KeyboardEvent& event, bool upOrDown);

        bool keyboardStatus[282]{false};
    };

    enum class mouseButtons : int { left = SDL_BUTTON_LEFT, middle=SDL_BUTTON_MIDDLE, right=SDL_BUTTON_RIGHT};
    enum class controllerButtons : int { invalid=-1, a=SDL_CONTROLLER_BUTTON_A, b=SDL_CONTROLLER_BUTTON_B, x=SDL_CONTROLLER_BUTTON_X, y=SDL_CONTROLLER_BUTTON_Y, back=SDL_CONTROLLER_BUTTON_BACK, guide=SDL_CONTROLLER_BUTTON_GUIDE, start=SDL_CONTROLLER_BUTTON_START};
    enum class controllerAxis : int { invalid=-1, leftX=SDL_CONTROLLER_AXIS_LEFTX, leftY=SDL_CONTROLLER_AXIS_LEFTY, rightX=SDL_CONTROLLER_AXIS_RIGHTX, rightY=SDL_CONTROLLER_AXIS_RIGHTY, triggerLeft=SDL_CONTROLLER_AXIS_TRIGGERLEFT, triggerRight=SDL_CONTROLLER_AXIS_TRIGGERRIGHT};

    class eventHandler
    {
        public:
        static eventHandler& get();

        void pollEvents();

        eventHandler& addInputDevice(inputDevice* device);

        private:
        SDL_Event event;
        std::vector<inputDevice*> devices;

        eventHandler() = default;

        eventHandler(const eventHandler& copyFrom) = delete;
        eventHandler& operator=(const eventHandler& copyFrom) = delete;

        eventHandler(eventHandler&& moveFrom) = delete;
        eventHandler& operator=(eventHandler&& moveFrom) = delete;

        ~eventHandler() = default;
   };
} //multiLib
