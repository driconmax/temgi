#pragma once

#include <temgi/Input.h>
#include <temgi/Memory.h>
#include <temgi/Graphics.h>

namespace temgi
{
    class Console
    {
        public:
            Console();

            Input& input();
            Memory& memory();
            Graphics& graphics();
            
        private:
            Input input_;
            Memory memory_;
            Graphics graphics_;
    };
    
} // namespace temgi
