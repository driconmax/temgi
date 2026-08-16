#include <iostream>

#include "GraphicsProcessor.h"

int main()
{
    temgi::GraphicsProcessor graphics;

    graphics.clear(123);

    const auto* pixels = graphics.framebuffer();

    std::cout << pixels[0] << '\n';
    std::cout << pixels[100] << '\n';
    std::cout << pixels[38399] << '\n';

    return 0;
}