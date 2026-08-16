#include "GraphicsProcessor.h"

#include <temgi/ConsoleSpec.h>

#include <algorithm>

namespace temgi
{
    void GraphicsProcessor::clear(Pixel color)
    {
        std::fill(
            framebuffer_.begin(),
            framebuffer_.end(),
            color
        );
    }

    void GraphicsProcessor::setPixel(std::uint16_t x, std::uint16_t y, Pixel color)
    {
        if(x >= ConsoleSpec::SCREEN_WIDTH || y >= ConsoleSpec::SCREEN_HEIGHT){
            return;
        }

        framebuffer_[y * ConsoleSpec::SCREEN_WIDTH + x] = color;
    }

    void GraphicsProcessor::drawSquare(std::uint16_t x, std::uint16_t y, std::uint16_t size_x, std::uint16_t size_y, Pixel color)
    {
        if(x >= ConsoleSpec::SCREEN_WIDTH || y >= ConsoleSpec::SCREEN_HEIGHT){
            return;
        }

        for (std::size_t ix = x; ix < x+size_x && ix < ConsoleSpec::SCREEN_WIDTH; ix++)
        {
            for (std::size_t iy = y; iy < y+size_y && iy < ConsoleSpec::SCREEN_HEIGHT; iy++)
            {
                setPixel(ix, iy, color);
            }
        }
    }

    const GraphicsProcessor::Pixel* GraphicsProcessor::framebuffer() const
    {
        return framebuffer_.data();
    }

} // namespace temgi
