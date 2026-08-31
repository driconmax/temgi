#include "temgi/Console.h"
#include "temgi/Asset.h"
#include "temgi/Colors.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace temgi
{
    Console::Console()
        : memoryManager_(),
        memory_(memoryManager_),
        graphicsProcessor_(),
        graphics_(graphicsProcessor_),
        input_()
    {
    }
    
    Input &Console::input()
    {
        return input_;
    }
    
    Memory &Console::memory()
    {
        return memory_;
    }
    
    Graphics &Console::graphics()
    {
        return graphics_;
    }

    void Console::subscribe(ConsoleEventSubscriber &subscriber)
    {
        subscribers_.push_back(&subscriber);
    }

    bool Console::loadCartridge(const std::string &path)
    {
        systemPrintLine("LOADING CARTRIDGE...");
        if(!cartridgeLoader_.load(path)){
            systemPrintLine("CARTRIDGE LOAD FAILED");
            presentSystemFrame();
            return false;
        }
        systemPrintLine("CARTRIDGE OK");
        presentSystemFrame();
        
        return true;
    }

    void Console::unloadCartridge()
    {
        if (cartridgeLoader_.cartridge() != nullptr)
        {
            cartridgeLoader_.unload();
        }
    }

    Asset Console::asset(const std::string &name) const
    {
        return cartridgeLoader_.asset(name);
    }

    void Console::run()
    {
        if (cartridgeLoader_.cartridge() == nullptr)
        {
            return;
        }
        running_ = true;

        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onConsoleStart();
        }

        systemClear();
        systemPrintLine("TEMGI");
        systemPrintLine("BOOTING...");
        systemPrintLine("MEMORY OK.");
        systemPrintLine("CARTRIDGE OK");
        systemPrintLine("STARTING GAME");
        
        presentSystemFrame();

        using Clock = std::chrono::steady_clock;
        std::this_thread::sleep_for(
            std::chrono::milliseconds(2000)
        );

        cartridgeLoader_.cartridge()->start(*this);
        
        auto previousFrame = Clock::now();
        
        while(running_){
            
            auto frameStart = Clock::now();

            float deltaTime =
                std::chrono::duration<float>(
                    frameStart - previousFrame
                ).count();

            previousFrame = frameStart;
            
            update(deltaTime);

            auto elapsed = Clock::now() - frameStart;

            if(elapsed < ConsoleSpec::FRAME_DURATION){
                std::this_thread::sleep_for(
                    ConsoleSpec::FRAME_DURATION - elapsed
                );
            }
        }

    }

    void Console::stop()
    {
        running_ = false;

        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onConsoleStop();
        }
    }

    void Console::update(float deltaTime)
    {
        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onFrameStart();
        }

        if (!failed_ && cartridgeLoader_.cartridge() != nullptr)
        {
            cartridgeLoader_.cartridge()->update(*this, deltaTime);
        }

        input_.nextFrame();

        if(error_) drawErrorOverlay();

        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onFrameEnd();
        }
    }

    void Console::drawErrorOverlay()
    {
        constexpr std::uint16_t margin = 8;

        graphicsProcessor_.drawSquare(margin, margin, ConsoleSpec::SCREEN_WIDTH - margin * 2, ConsoleSpec::SCREEN_HEIGHT - margin * 2, Colors::Black);

        graphicsProcessor_.drawText("TEMGI ERROR", 16, 16, Colors::Red);
        graphicsProcessor_.drawText(errorMessage_, 16, 32, Colors::White);
    }

    void Console::systemClear()
    {
        graphics_.clear(0x00);
        systemCursorY_ = 0;
    }

    void Console::systemPrintLine(const std::string &text)
    {
        graphics_.drawText(text, 8, systemCursorY_, 0xFF);
        systemCursorY_ += 9;
    }

    void Console::presentSystemFrame()
    {
        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onFrameStart();
        }

        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onFrameEnd();
        }
    }

    void Console::setButton(Button button, bool pressed)
    {
        input_.setButton(button, pressed);
    }

    void Console::error(const std::string &message)
    {
        error_ = true;
        errorMessage_ = message;
        
        std::cerr << "[TEMGI ERROR] " << message << '\n';
    }

    void Console::fatalError(const std::string &message)
    {
        if(failed_){
            return;
        }

        failed_ = true;
        errorMessage_ = message;

        drawErrorOverlay();
    }

    const Graphics::Pixel *Console::frameBuffer() const
    {
        return graphics_.frameBuffer();
    }

} // namespace temgi
