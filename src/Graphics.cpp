#include <temgi/Graphics.h>

#include <temgi/Font.h>

#include <temgi/internal/BitmapFont.h>
#include "temgi/internal/GraphicsProcessor.h"

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

    void Graphics::drawImage(const Image &image, std::uint16_t x, std::uint16_t y, Pivot pivot)
    {
        processor_.drawImage(image, x, y, pivot);
    }

    void Graphics::drawAnimationFrame(const Animation &animation, std::uint16_t frame, std::uint16_t x, std::uint16_t y, Pivot pivot)
    {
        processor_.drawAnimationFrame(
            animation,
            frame,
            x,
            y,
            pivot
        );
    }

    void Graphics::drawText(const std::string &text, std::uint16_t x, std::uint16_t y, Pixel color)
    {
        processor_.drawText(text, x, y, color, BitmapFont::font());
    }

    void Graphics::drawText(const std::string &text, std::uint16_t x, std::uint16_t y, Pixel color, const Font& font)
    {
        processor_.drawText(text, x, y, color, font);
    }

    std::uint16_t Graphics::measureText(const std::string& text) const
    {
        return processor_.measureText(text, BitmapFont::font());
    }

    std::uint16_t Graphics::measureText(const std::string& text, const Font& font) const
    {
        return processor_.measureText(text, font);
    }

    const Graphics::Pixel* Graphics::frameBuffer() const
    {
        return processor_.framebuffer();
    }
}