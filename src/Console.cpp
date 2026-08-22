#include "Console.h"

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

        cartridgeLoader_.cartridge()->start(*this);

        return false;
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
