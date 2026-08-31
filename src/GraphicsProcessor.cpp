#include "temgi/internal/GraphicsProcessor.h"
#include "temgi/internal/BitmapFont.h"

#include <temgi/ConsoleSpec.h>

#include <algorithm>
#include <string>

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

    void GraphicsProcessor::drawAnimationFrame(
        const Animation& animation,
        std::uint16_t frame,
        std::uint16_t x,
        std::uint16_t y)
    {
        if (frame >= animation.frameCount)
        {
            return;
        }

        const std::size_t frameSize =
            static_cast<std::size_t>(animation.width) *
            static_cast<std::size_t>(animation.height);

        const Pixel* framePixels =
            animation.pixels +
            frameSize * frame;

        for (std::uint16_t iy = 0; iy < animation.height; ++iy)
        {
            for (std::uint16_t ix = 0; ix < animation.width; ++ix)
            {
                const std::size_t index =
                    static_cast<std::size_t>(iy) *
                    animation.width +
                    ix;

                Pixel pixel = framePixels[index];

                if (pixel == 0x00)
                {
                    continue;
                }

                setPixel(
                    x + ix,
                    y + iy,
                    pixel
                );
            }
        }
    }

    void GraphicsProcessor::drawChar(char character, std::uint16_t x, std::uint16_t y, Pixel color, const Font& font)
    {
        const auto code = static_cast<std::uint8_t>(character);

        std::size_t glyphIndex = 0;
        bool found = false;

        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
        for (std::uint8_t i = 0; i < font.characterCount; ++i) {
            if (font.characters[i] == character)
            {
                glyphIndex = i;
                found = true;
                break;
            }
        }

        if (!found)
        {
            for (std::uint8_t i = 0; i < font.characterCount; ++i)
            {
                if (font.characters[i] == '?')
                {
                    glyphIndex = i;
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            return;
        }

        const std::size_t glyphOffset =
            glyphIndex *
            font.height;

        const std::uint8_t* glyph =
            font.glyphs +
            glyphOffset;

        for (
            std::uint8_t row = 0;
            row < font.height;
            ++row
        )
        {
            const std::uint8_t rowBits =
                glyph[row];

            for (
                std::uint8_t column = 0;
                column < font.width;
                ++column
            )
            {
                const std::uint8_t bit =
                    font.width -
                    1 -
                    column;

                if (rowBits & (1 << bit))
                {
                    setPixel(
                        x + column,
                        y + row,
                        color
                    );
                }
            }
        }
        
    }

    void GraphicsProcessor::drawText(const std::string& text, std::uint16_t x, std::uint16_t y, Pixel color){
        drawText(text, x, y, color, BitmapFont::font());
    }

    void GraphicsProcessor::drawText(const std::string& text, std::uint16_t x, std::uint16_t y, Pixel color, const Font& font)
    {
        std::uint16_t cursorX = x;
        std::uint16_t cursorY = y;

        constexpr std::uint16_t SPACING = 1;

        for (char character : text)
        {
            if (character == '\n')
            {
                cursorX = x;

                cursorY += font.height + SPACING;

                continue;
            }

            drawChar(character, cursorX, cursorY, color, font);

            cursorX += font.width + SPACING;
        }
    }

    const GraphicsProcessor::Pixel* GraphicsProcessor::framebuffer() const
    {
        return framebuffer_.data();
    }

} // namespace temgi
