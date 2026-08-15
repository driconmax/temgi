#pragma once

#include <cstddef>

namespace temgi {
    class MemoryManager;

    class Memory
    {
        public:
            void* allocateMain(std::size_t bytes);
            void* allocateFast(std::size_t bytes);

            std::size_t mainUsed() const;
            std::size_t mainAvailable() const;

            std::size_t fastUsed() const;
            std::size_t fastAvailable() const;

        private:
            friend class MemoryManager;

            explicit Memory(MemoryManager& manager);

            MemoryManager& manager_;
    };
}