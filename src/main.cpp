#include "multiLib/misc.h"
#include "multiLib/visuals.h"
#include "multiLib/audio.h"
#include "multiLib/events.h"
#include "networking.h"

#include <thread>
#include <chrono>

int main()
{
#define IS_SERVER 0

#if IS_SERVER == 1
    std::cout << "SERVER" << std::endl;

    networking::tcpServer server{"4444"};
    server.bind();

    std::unique_ptr<networking::baseClient> serverClient = server.acceptClient();

    std::cout << "GOT CLIENT" << std::endl;

    serverClient->send("HELLO SUSSY BAKA");

    std::string buffer{};

    serverClient->receive(buffer, 100);

    std::cout << buffer << std::endl;
#else
    std::cout << "CLIENT" << std::endl;

    networking::tcpClient client{};

    client.connect("127.0.0.1", "4444");

    std::string buffer{};

    client.receive(buffer, 16);

    client.send("HELLO TO YOU TO");

    std::cout << buffer << std::endl;
#endif

    Estd::initGuard<std::function<void(void)>> SDL_Guard = multiLib::initSDL();
    Estd::initGuard<std::function<void(void)>> Image_Guard = multiLib::initImage();
    Estd::initGuard<std::function<void(void)>> TTF_Guard = multiLib::initTTF();
    Estd::initGuard<std::function<void(void)>> Mixer_Guard = multiLib::initMixer();
    Estd::initGuard<std::function<void(void)>> Audio_Guard = multiLib::openAudio();

    multiLib::renderWindow mainWindow{"demo", 1280, 720};
    mainWindow.fullScreenWindow();

    mainWindow.setIcon("assets/icon.png");

    multiLib::eventMessager mainEvent{};

    multiLib::keyboardEvent mainKeyboard{};

    multiLib::mouseEvent mainMouse{};

    float x{}, y{}, speed{10};

    bool mouseDown{false};

    multiLib::image player{"assets/icon.png", multiLib::rectangle{0, 0, 600, 600}, mainWindow};

    auto oldTime = std::chrono::steady_clock::now();
    double deltaTime = 0.0;

    mainKeyboard.keyEventCallback( [&x, &y, speed, &deltaTime] (multiLib::keyboardKeys key, bool upOrDown)
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

                default:
                    break;
            }
        }
    });

    mainEvent.addEventHandler(&mainKeyboard);

    mainMouse.mouseButtonCallback( [&mouseDown](bool upOrDown, multiLib::mouseButtons, int, int)
    {
        mouseDown = upOrDown;
    });

    mainMouse.mouseMoveCallback( [&mouseDown, &x, &y](int mouseX, int mouseY, int, int)
    {
        if(mouseDown)
        {
            x = mouseX;
            y = mouseY;
        }
    });

    mainEvent.addEventHandler(&mainMouse);

    while(true)
    {
        auto currentTime = std::chrono::steady_clock::now();
        deltaTime = -(double)std::chrono::duration_cast<std::chrono::milliseconds>(oldTime - currentTime).count() / 10;

        oldTime = currentTime;

        mainWindow.clear();

        mainWindow.draw(player, x, y, 100, 100);

        mainEvent.pollEvents();

        mainWindow.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
