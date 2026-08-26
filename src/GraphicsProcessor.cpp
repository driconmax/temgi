#include "temgi/internal/GraphicsProcessor.h"

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

        if(x < 0) x = 0;
        if(y < 0) y = 0;

        for (std::size_t ix = x; ix < x+size_x && ix < ConsoleSpec::SCREEN_WIDTH; ix++)
        {
            for (std::size_t iy = y; iy < y+size_y && iy < ConsoleSpec::SCREEN_HEIGHT; iy++)
            {
                setPixel(ix, iy, color);
            }
        }
    }

    void GraphicsProcessor::drawImage(const Image &image, std::uint16_t x, std::uint16_t y)
    {
        for (std::uint16_t iy = 0; iy < image.height; iy++)
        {
            for (std::uint16_t ix = 0; ix < image.width; ix++)
            {
                std::size_t index = static_cast<std::size_t>(iy) * image.width + ix;

                Pixel pixel = image.pixels[index];

                if(pixel == 0x00) continue;

                setPixel(x + ix, y + iy, pixel);
            }
        }
        
    }

    const GraphicsProcessor::Pixel* GraphicsProcessor::framebuffer() const
    {
        return framebuffer_.data();
    }

} // namespace temgi
