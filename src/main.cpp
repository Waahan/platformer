#include "multiLib.h"

int main()
{
    multiLib::init& mainInit = multiLib::init::getInit();

    mainInit.initSystem(multiLib::initSystems::SDL).initSystem(multiLib::initSystems::Image).initSystem(multiLib::initSystems::TTF).initSystem(multiLib::initSystems::Mixer);

    multiLib::renderWindow mainWindow{"Test", 800, 800};

    return 0;
}
