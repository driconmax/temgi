#include "temgi/Console.h"
#include "temgi/Asset.h"

#include <thread>
#include <chrono>

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
        if(!cartridgeLoader_.load(path)){
            return false;
        }

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

        using Clock = std::chrono::steady_clock;

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

        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onFrameEnd();
        }
    }

    void Console::drawErrorOverlay()
    {
        constexpr std::uint8_t BLACK = 0x01;
        constexpr std::uint8_t WHITE = 0xD8;
        constexpr std::uint8_t RED = 0xB5;

        constexpr std::uint16_t margin = 8;

        graphicsProcessor_.drawSquare(margin, margin, ConsoleSpec::SCREEN_WIDTH - margin * 2, ConsoleSpec::SCREEN_HEIGHT - margin * 2, BLACK);

        graphicsProcessor_.drawText("TEMGI ERROR", 16, 16, RED);
        graphicsProcessor_.drawText(errorMessage_, 16, 32, WHITE);
    }

    void Console::setButton(Button button, bool pressed)
    {
        input_.setButton(button, pressed);
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

    const std::uint8_t *Console::frameBuffer() const
    {
        return graphics_.frameBuffer();
    }

} // namespace temgi
