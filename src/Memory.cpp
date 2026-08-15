#include <temgi/Memory.h>

#include "MemoryManager.h"
#include "Memory.h"

namespace temgi {
    Memory::Memory(MemoryManager& manager) : manager_(manager) {

    }

    void* Memory::allocateMain(std::size_t bytes, std::size_t alignment) {
        return manager_.allocateMain(bytes, alignment);
    }

    void *temgi::Memory::allocateFast(std::size_t bytes, std::size_t alignment)
    {
        return manager_.allocateFast(bytes, alignment);
    }
}
