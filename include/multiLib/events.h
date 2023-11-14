#pragma once

#include <vector>
#include <functional>

extern "C"
{
    #include <SDL2/SDL.h>
}

namespace multiLib
{
    class eventHandler
    {
        public:
        eventHandler() = default;

        eventHandler(const eventHandler& copyFrom) = delete;
        eventHandler& operator=(const eventHandler& copyFrom) = delete;

        eventHandler(eventHandler&& moveFrom) = delete;
        eventHandler& operator=(eventHandler&& moveFrom) = delete;

        virtual ~eventHandler() = default;

        virtual bool useEvent(const SDL_Event& event) =0;
    };

    class eventMessager
    {
        public:
        eventMessager();

        eventMessager(const eventMessager& copyFrom) = delete;
        eventMessager& operator=(const eventMessager& copyFrom) = delete;

        eventMessager(eventMessager&& moveFrom) = default;
        eventMessager& operator=(eventMessager&& moveFrom) = default;

        ~eventMessager() = default;

        void pollEvents();

        void setExitCallback(std::function<void(void)>&& exitFunc);
        eventMessager& addEventHandler(eventHandler* add);

        private:
        std::vector<eventHandler*> eventHandlers;
        std::function<void(void)> exitFunction;
    };

    enum class keyboardKeys : int
    {
        a=SDL_SCANCODE_A,
        b=SDL_SCANCODE_B,
        c=SDL_SCANCODE_C,
        d=SDL_SCANCODE_D,
        e=SDL_SCANCODE_E,
        f=SDL_SCANCODE_F,
        g=SDL_SCANCODE_G,
        h=SDL_SCANCODE_H,
        i=SDL_SCANCODE_I,
        j=SDL_SCANCODE_J,
        k=SDL_SCANCODE_K,
        l=SDL_SCANCODE_L,
        m=SDL_SCANCODE_M,
        n=SDL_SCANCODE_N,
        o=SDL_SCANCODE_O,
        p=SDL_SCANCODE_P,
        q=SDL_SCANCODE_Q,
        r=SDL_SCANCODE_R,
        s=SDL_SCANCODE_S,
        t=SDL_SCANCODE_T,
        u=SDL_SCANCODE_U,
        v=SDL_SCANCODE_V,
        w=SDL_SCANCODE_W,
        x=SDL_SCANCODE_X,
        y=SDL_SCANCODE_Y,
        z=SDL_SCANCODE_Z,

        one=SDL_SCANCODE_1,
        two=SDL_SCANCODE_2,
        three=SDL_SCANCODE_3,
        four=SDL_SCANCODE_4,
        five=SDL_SCANCODE_5,
        six=SDL_SCANCODE_6,
        seven=SDL_SCANCODE_7,
        eight=SDL_SCANCODE_8,
        nine=SDL_SCANCODE_9,
        zero=SDL_SCANCODE_0,

        escape=SDL_SCANCODE_ESCAPE,
        backspace=SDL_SCANCODE_BACKSPACE,
        tab=SDL_SCANCODE_TAB,
        space=SDL_SCANCODE_SPACE,
    };

    class keyboardEvent : public eventHandler
    {
        public:
        keyboardEvent() = default;

        keyboardEvent(const keyboardEvent& copyFrom) = default;
        keyboardEvent& operator=(const keyboardEvent& copyFrom) = default;

        keyboardEvent(keyboardEvent&& moveFrom) = default;
        keyboardEvent& operator=(keyboardEvent&& moveFrom) = default;

        ~keyboardEvent() = default;

        bool useEvent(const SDL_Event& event) override;

        void keyEventCallback(std::function<void(keyboardKeys, bool)>&& keyCallback);

        private:
        void handleKeyEvents(const SDL_KeyboardEvent& event, bool upOrDown);

        std::function<void(keyboardKeys, bool)> keyCallFunc;
    };

    enum class mouseButtons : int
    {
        left = SDL_BUTTON_LEFT,
        middle = SDL_BUTTON_MIDDLE,
        right = SDL_BUTTON_RIGHT,
        other1 = SDL_BUTTON_X1,
        other2 = SDL_BUTTON_X2,
    };

    class mouseEvent : public eventHandler
    {
        public:
        mouseEvent() = default;

        mouseEvent(const mouseEvent& copyFrom) = default;
        mouseEvent& operator=(const mouseEvent& copyFrom) = default;

        mouseEvent(mouseEvent&& moveFrom) = default;
        mouseEvent& operator=(mouseEvent&& moveFrom) = default;

        ~mouseEvent() = default;

        bool useEvent(const SDL_Event& event) override;

        void mouseButtonCallback(std::function<void(bool, mouseButtons, int, int)>&& setCallback);
        void mouseMoveCallback(std::function<void(int, int, int, int)>&& setCallback);

        private:
        void handleMouseButtonEvents(const SDL_MouseButtonEvent& event, bool upOrDown);
        void handleMouseMotionEvents(const SDL_MouseMotionEvent& event);

        std::function<void(bool, mouseButtons, int, int)> mouseButtonCallFunc;
        std::function<void(int, int, int, int)> mouseMoveCallFunc;
    };
} //multiLib
