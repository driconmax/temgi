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

        if (cartridgeLoader_.cartridge() != nullptr)
        {
            cartridgeLoader_.cartridge()->update(*this, deltaTime);
        }

        input_.nextFrame();

        for (ConsoleEventSubscriber* subscriber : subscribers_)
        {
            subscriber->onFrameEnd();
        }
    }

    void Console::setButton(Button button, bool pressed)
    {
        input_.setButton(button, pressed);
    }

    const std::uint8_t *Console::frameBuffer() const
    {
        return graphics_.frameBuffer();
    }

} // namespace temgi
