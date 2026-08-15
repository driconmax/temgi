#include <temgi/Memory.h>

#include "MemoryManager.h"
#include "Memory.h"

namespace temgi {
    Memory::Memory(MemoryManager& manager) : manager_(manager) {

    }

    void* Memory::allocateMain(std::size_t bytes) {
        return manager_.allocateMain(bytes);
    }

    void *temgi::Memory::allocateFast(std::size_t bytes)
    {
        return manager_.allocateFast(bytes);
    }
}
