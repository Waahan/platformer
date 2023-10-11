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
} //multiLib
