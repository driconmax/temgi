#include <temgi/Graphics.h>

#include "GraphicsProcessor.h"

namespace temgi
{
    Graphics::Graphics(GraphicsProcessor& processor)
        : processor_(processor)
    {
    }

    void Graphics::clear(Pixel color)
    {
        processor_.clear(color);
    }

    void Graphics::setPixel(
        std::uint16_t x,
        std::uint16_t y,
        Pixel color)
    {
        processor_.setPixel(x, y, color);
    }

    void Graphics::drawSquare(
        std::uint16_t x,
        std::uint16_t y,
        std::uint16_t width,
        std::uint16_t height,
        Pixel color)
    {
        processor_.drawSquare(x, y, width, height, color);
    }

    const GraphicsProcessor::Pixel* Graphics::frameBuffer() const
    {
        return processor_.framebuffer();
    }
}