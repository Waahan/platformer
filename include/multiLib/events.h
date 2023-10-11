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

        ~eventHandler() = default;

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
} //multiLib
