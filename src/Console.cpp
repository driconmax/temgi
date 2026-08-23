#include "Console.h"

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

    void Console::run()
    {
        if (cartridgeLoader_.cartridge() == nullptr)
        {
            return;
        }
        running_ = true;

        using Clock = std::chrono::steady_clock;

        cartridgeLoader_.cartridge()->start(*this);
        

        while(running_){
            
            auto start = Clock::now();
            
            update();
            auto elapsed = Clock::now() - start;

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
    }

    void Console::update()
    {
        if (cartridgeLoader_.cartridge() != nullptr)
        {
            cartridgeLoader_.cartridge()->update(*this);
        }

        input_.nextFrame();
    }

    void Console::setButton(Button button, bool pressed)
    {
        input_.setButton(button, pressed);
    }

    const std::uint32_t *Console::frameBuffer() const
    {
        return graphics_.frameBuffer();
    }

} // namespace temgi
