#pragma once

#include <temgi/Input.h>
#include <temgi/Memory.h>
#include "MemoryManager.h"
#include <temgi/Graphics.h>
#include "GraphicsProcessor.h"

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
            
            MemoryManager memoryManager_;
            Memory memory_;

            GraphicsProcessor graphicsProcessor_;
            Graphics graphics_;
    };
    
} // namespace temgi
