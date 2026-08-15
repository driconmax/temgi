#pragma once

#include "MemoryRegion.h"

namespace temgi {
    class MemoryManager {
        public:
            MemoryManager();
            void* allocateMain(std::size_t bytes);
            void* allocateFast(std::size_t bytes);
            std::size_t mainUsed() const;
            std::size_t fastUsed() const;

        private:
            MemoryRegion fastRam_;
            MemoryRegion mainRam_;
            MemoryRegion videoRam_;
            MemoryRegion spriteRam_;
            MemoryRegion paletteRam_;
            MemoryRegion saveRam_;
    };
}