#include "Console.h"

namespace temgi
{
    
    
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
