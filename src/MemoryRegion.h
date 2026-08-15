#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace temgi {
    class MemoryRegion {
        public:
            MemoryRegion(std::string name, std::size_t capacity);

            std::size_t capacity() const;
            std::size_t used() const;
            std::size_t available() const;

        private:
            std::string name_;
            std::vector<std::byte> data_;
            std::size_t used_ = 0;
    };
}