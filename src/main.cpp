#include "multiLib/misc.h"
#include "multiLib/visuals.h"
#include "multiLib/events.h"

int main()
{
    multiLib::init& mainInit = multiLib::init::getInit();

    mainInit.initSystem(multiLib::initSystems::SDL).initSystem(multiLib::initSystems::Image).initSystem(multiLib::initSystems::TTF).initSystem(multiLib::initSystems::Mixer);

    multiLib::renderWindow mainWindow{"Test", 800, 800};

    multiLib::eventMessager mainEvent{};

    multiLib::keyboardEvent mainKeyboard{};

    mainKeyboard.keyEventCallback([](multiLib::keyboardKeys key, bool upOrDown){ if(key == multiLib::keyboardKeys::escape) exit(0); });

    mainEvent.addEventHandler(&mainKeyboard);

    while(true)
    {
        mainEvent.pollEvents();
    }

    return 0;
}
