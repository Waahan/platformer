#include "multiLib/misc.h"
#include "multiLib/visuals.h"
#include "multiLib/audio.h"
#include "multiLib/events.h"

#include <thread>

int main()
{
    multiLib::initGuard<std::function<void(void)>> SDL_Guard = multiLib::initSDL();
    multiLib::initGuard<std::function<void(void)>> Image_Guard = multiLib::initImage();
    multiLib::initGuard<std::function<void(void)>> TTF_Guard = multiLib::initTTF();
    multiLib::initGuard<std::function<void(void)>> Mixer_Guard = multiLib::initMixer();
    multiLib::initGuard<std::function<void(void)>> Audio_Guard = multiLib::openAudio();

    multiLib::renderWindow mainWindow{"demo", 1280, 720};
    mainWindow.fullScreenWindow();

    mainWindow.setIcon("assets/icon.png");

    multiLib::eventMessager mainEvent{};

    multiLib::keyboardEvent mainKeyboard{};

    float x{}, y{}, speed{10};

    multiLib::image player{"assets/icon.png", multiLib::rectangle{0, 0, 600, 600}, mainWindow};

    mainKeyboard.keyEventCallback( [&x, &y, speed] (multiLib::keyboardKeys key, bool upOrDown)
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

                default:
                    break;
            }
        }
    });

    mainEvent.addEventHandler(&mainKeyboard);

    while(true)
    {
        mainWindow.clear();

        mainWindow.draw(player, x, y, 100, 100);

        mainEvent.pollEvents();

        mainWindow.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
