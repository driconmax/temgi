
#include "temgi/internal/MemoryManager.h"

#include <temgi/ConsoleSpec.h>

namespace temgi
{
    MemoryManager::MemoryManager() :
        fastRam_("Fast RAM", ConsoleSpec::FAST_RAM_SIZE),
        mainRam_("Fast RAM", ConsoleSpec::MAIN_RAM_SIZE),
        videoRam_("Fast RAM", ConsoleSpec::VIDEO_RAM_SIZE),
        spriteRam_("Fast RAM", ConsoleSpec::SPRITE_RAM_SIZE),
        paletteRam_("Fast RAM", ConsoleSpec::PALETTE_RAM_SIZE),
        saveRam_("Fast RAM", ConsoleSpec::SAVE_RAM_SIZE)
    {
        
    }

    void *MemoryManager::allocateMain(std::size_t bytes, std::size_t alignment)
    {
        return mainRam_.allocate(bytes, alignment);
    }

    void *MemoryManager::allocateFast(std::size_t bytes, std::size_t alignment)
    {
        return fastRam_.allocate(bytes, alignment);
    }

    std::size_t MemoryManager::mainUsed() const{
        return mainRam_.used();
    }

    std::size_t MemoryManager::fastUsed() const{
        return fastRam_.used();
    }



} // namespace temgi
