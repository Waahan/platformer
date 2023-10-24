#include "multiLib/misc.h"
#include "multiLib/visuals.h"
#include "multiLib/audio.h"
#include "multiLib/events.h"

#include <thread>

int main()
{
    multiLib::init& mainInit = multiLib::init::getInit();

    mainInit.initSystem(multiLib::initSystems::SDL).initSystem(multiLib::initSystems::Image).initSystem(multiLib::initSystems::TTF).initSystem(multiLib::initSystems::Mixer);

    multiLib::renderWindow mainWindow{"demo", 1280, 720};
    mainWindow.fullScreenWindow();

    mainWindow.setIcon("assets/icon.png");

    multiLib::eventMessager mainEvent{};

    multiLib::keyboardEvent mainKeyboard{};

    multiLib::music coolMusic{"assets/music.mp4"};

    coolMusic.play();

    float x{}, y{}, speed{0.5f};

    auto prev = std::chrono::steady_clock::now();

    float deltaTime = 0.0f;

    multiLib::image player{"assets/icon.png", multiLib::rectangle{0, 0, 600, 600}, mainWindow};

    mainKeyboard.keyEventCallback( [&x, &y, speed, &coolMusic, &deltaTime] (multiLib::keyboardKeys key, bool upOrDown)
    { 
        if(upOrDown)
        {
            switch(key)
            {
                case multiLib::keyboardKeys::escape:
                    exit(0);
                case multiLib::keyboardKeys::w:
                    y += speed * deltaTime;
                    break;

                case multiLib::keyboardKeys::s:
                    y -= speed * deltaTime;
                    break;

                case multiLib::keyboardKeys::a: 
                    x -= speed * deltaTime; 
                    break;
                    
                case multiLib::keyboardKeys::d:
                    x += speed * deltaTime;
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
        deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - prev).count() / 100000;
        prev = std::chrono::steady_clock::now();

        mainWindow.clear();

        mainWindow.draw(player, x, y, 100, 100);

        mainEvent.pollEvents();

        mainWindow.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
