#include "multiLib/events.h"

#include "errors.h"

namespace multiLib
{
    eventMessager::eventMessager()
        : eventHandlers{}, exitFunction{[](){ exit(0); }}
    {
    /*
        Construct an eventMessager with a default exitFunction
    */
    }

    void eventMessager::pollEvents()
    {
    /*
        Poll user events and loop over eventHandlers till one claims the event
    */
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
    /*
        Replace the default exitFunction
    */
        exitFunction = std::move(exitFunc);
    }

    eventMessager& eventMessager::addEventHandler(eventHandler* add)
    {
    /*
        Add an eventHandler

        Precondition add is not a nullptr
    */
        debug_assert((add), "eventMessager eventHandler can not be nullptr");

        eventHandlers.push_back(add);

        return *this;
    }

    bool keyboardEvent::useEvent(const SDL_Event& event)
    {
    /*
        Claim and handle keyboard events
    */
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
    /*
        Set a keyboard callback
    */
        keyCallFunc = std::move(keyCallback);
    }

    void keyboardEvent::handleKeyEvents(const SDL_KeyboardEvent& event, bool upOrDown)
    {
    /*
        handle SDL_KeyboardEvent
    */
        keyCallFunc(static_cast<keyboardKeys>(event.keysym.scancode), upOrDown);
    }

    bool mouseEvent::useEvent(const SDL_Event& event)
    {
    /*
        Claim and handle mouse events
    */
        switch(event.type)
        {
            case SDL_MOUSEMOTION:
                handleMouseMotionEvents(event.motion);
                return true;

            case SDL_MOUSEBUTTONDOWN:
                handleMouseButtonEvents(event.button, true);
                return true;

            case SDL_MOUSEBUTTONUP:
                handleMouseButtonEvents(event.button, false);
                return true;

            case SDL_MOUSEWHEEL:
                return true;
        }

        return false;
    }

    void mouseEvent::mouseButtonCallback(std::function<void(bool, mouseButtons, int, int)>&& setCallback)
    {
    /*
        Set the callback for mouse button
    */
        mouseButtonCallFunc = std::move(setCallback);
    }
    
    void mouseEvent::mouseMoveCallback(std::function<void(int, int, int, int)>&& setCallback)
    {
    /*
        Set the callback for mouse movement
    */
        mouseMoveCallFunc = std::move(setCallback);
    }

    void mouseEvent::handleMouseButtonEvents(const SDL_MouseButtonEvent& event, bool upOrDown)
    {
    /*
        Handle mouse button events
    */
        mouseButtonCallFunc(upOrDown, static_cast<mouseButtons>(event.button), event.x, event.y); 
    }

    void mouseEvent::handleMouseMotionEvents(const SDL_MouseMotionEvent& event)
    {
    /*
        Handle mouse motion events
    */
        mouseMoveCallFunc(event.x, -event.y, event.xrel, event.yrel);
    }
} //multiLib
