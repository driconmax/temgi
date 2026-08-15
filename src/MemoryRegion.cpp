#include "MemoryRegion.h"

#include <utility>

namespace temgi
{
    MemoryRegion::MemoryRegion(std::string name, std::size_t capacity) : name_(std::move(name)), data_(capacity) {

    }

    void *MemoryRegion::allocate(std::size_t bytes)
    {
        if(used_ + bytes > data_.size()){
            return nullptr;
        }

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
