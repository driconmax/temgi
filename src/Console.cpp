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
    
} // namespace temgi
