#include <iostream>

#include "temgi/internal/MemoryManager.h"

int main(int argc, char const *argv[])
{
    temgi::MemoryManager memory;

    void* main = memory.allocateMain(100);
    void* fast = memory.allocateFast(200);
    
    std::cout << "Main: " << main << '\n';
    std::cout << "Fast: " << fast << '\n';

    fast = memory.allocateFast(500 * 1024);
    std::cout << "Overflow?: " <<  fast << '\n';

    return 0;
}
