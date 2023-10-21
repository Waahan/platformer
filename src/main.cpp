#include "multiLib/misc.h"
#include "multiLib/visuals.h"
#include "multiLib/audio.h"
#include "multiLib/events.h"

#include <cassert>

int main()
{
    multiLib::init& mainInit = multiLib::init::getInit();

    mainInit.initSystem(multiLib::initSystems::SDL).initSystem(multiLib::initSystems::Image).initSystem(multiLib::initSystems::TTF).initSystem(multiLib::initSystems::Mixer);

    multiLib::renderWindow mainWindow{"demo", 1280, 720};

    mainWindow.setIcon("assets/icon.png");

    multiLib::eventMessager mainEvent{};

    multiLib::keyboardEvent mainKeyboard{};

    multiLib::music coolMusic{"assets/music.mp4"};

    coolMusic.play();

    int x{}, y{}, speed{50};

    multiLib::image player{"assets/icon.png", multiLib::rectangle{0, 0, 600, 600}, mainWindow};

    mainKeyboard.keyEventCallback( [&x, &y, speed, &coolMusic](multiLib::keyboardKeys key, bool upOrDown)
    { 
        if(upOrDown)
        {
            switch(key)
            {
                case multiLib::keyboardKeys::escape:
                    exit(0);
                case multiLib::keyboardKeys::w:
                    y += speed;
                    break;

                case multiLib::keyboardKeys::s:
                    y -= speed;
                    break;

                case multiLib::keyboardKeys::a: 
                    x -= speed;
                    break;
                    
                case multiLib::keyboardKeys::d:
                    x += speed;
                    break;

                case multiLib::keyboardKeys::m:
                    coolMusic.playing() ? coolMusic.pause() : coolMusic.resume();
                    break;

                default:
                    break;
            }
        }
    });

    mainEvent.addEventHandler(&mainKeyboard);

    while(true)
    {
        mainWindow.clear();

        mainWindow.draw(player, x, y);

        mainEvent.pollEvents();

        mainWindow.display();
    }

    return 0;
}
