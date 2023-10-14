#include "multiLib/events.h"

#include <cassert>

namespace multiLib
{
    eventMessager::eventMessager()
        : eventHandlers{}, exitFunction{[](){ exit(0); }}
    {
    }

    void eventMessager::pollEvents()
    {
        SDL_Event event; 

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                exitFunction();
            }

            for(const auto& eventHandle : eventHandlers)
            {
                if(eventHandle->useEvent(event))
                    break;
            }
        }
    }

    void eventMessager::setExitCallback(std::function<void(void)>&& exitFunc)
    {
        exitFunction = std::move(exitFunc);
    }

    eventMessager& eventMessager::addEventHandler(eventHandler* add)
    {
    /*
        Precondition add is not a nullptr
    */
        assert(add && "eventMessager eventHandler can not be nullptr");

        eventHandlers.push_back(add);

        return *this;
    }

    bool keyboardEvent::useEvent(const SDL_Event& event)
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                handleKeyEvents(event.key, true);
                return true;

            case SDL_KEYUP:
                handleKeyEvents(event.key, false);
                return true;
            
            case SDL_TEXTINPUT:
                return true;

            case SDL_TEXTEDITING:
                return true;
        }

        return false;
    }

    void keyboardEvent::keyEventCallback(std::function<void(keyboardKeys, bool)>&& keyCallback)
    {
        keyCallFunc = std::move(keyCallback);
    }

    void keyboardEvent::handleKeyEvents(const SDL_KeyboardEvent& event, bool upOrDown)
    {
        keyCallFunc(static_cast<keyboardKeys>(event.keysym.scancode), upOrDown);
    }
} //multiLib
