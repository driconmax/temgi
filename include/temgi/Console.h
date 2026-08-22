#pragma once

#include <cstdint>
#include <string>

#include <temgi/Input.h>
#include <temgi/Memory.h>
#include <temgi/Graphics.h>

#include "MemoryManager.h"
#include "GraphicsProcessor.h"
#include "CartridgeLoader.h"

namespace temgi
{
    class Console
    {
        public:
            Console();

            Input& input();
            Memory& memory();
            Graphics& graphics();

            bool loadCartridge(const std::string& path);

            void setButton(Button button, bool pressed);

            const std::uint32_t* frameBuffer() const;

            void update();
            
        private:
            Input input_;
            
            MemoryManager memoryManager_;
            Memory memory_;

            GraphicsProcessor graphicsProcessor_;
            Graphics graphics_;

            CartridgeLoader cartridgeLoader_;
    };
    
} // namespace temgi
