
#include "temgi/internal/MemoryRegion.h"

#include <cstdint>
#include <utility>

namespace temgi
{
    MemoryRegion::MemoryRegion(std::string name, std::size_t capacity) : name_(std::move(name)), data_(capacity) {

    }

    void *MemoryRegion::allocate(std::size_t bytes, std::size_t alignment)
    {

        std::byte* current = data_.data() + used_;

        std::uintptr_t address = reinterpret_cast<std::uintptr_t>(current);
        std::size_t padding = (alignment - (address % alignment)) % alignment;

        if(used_ + padding + bytes > data_.size()){
            return nullptr;
        }

        used_ += padding;

        void* result = data_.data() + used_;

        used_ += bytes;

        return result;
    }

    std::size_t MemoryRegion::capacity() const {
        return data_.size();
    }

    std::size_t MemoryRegion::used() const {
        return used_;
    }

    std::size_t MemoryRegion::available() const {
        return capacity() - used();
    }


} // namespace temgi
