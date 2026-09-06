#pragma once

#include <array>
#include <cstdint>

#include <temgi/ConsoleSpec.h>
#include <temgi/Image.h>
#include <temgi/Animation.h>
#include <temgi/Font.h>
#include <temgi/PixelFormat.h>
#include <temgi/Pivot.h>

namespace temgi
{
    class GraphicsProcessor
    {
        
        public:
            using Pixel = PixelStorage;
            
            void clear(Pixel color);
            void setPixel(std::uint16_t x, std::uint16_t y, Pixel color);
            void drawSquare(std::uint16_t x, std::uint16_t y, std::uint16_t size_x, std::uint16_t size_y, Pixel color);

            void drawImage(
                const Image& image,
                std::uint16_t x,
                std::uint16_t y,
                Pivot pivot = Pivot::TopLeft
            );

            void drawAnimationFrame(const Animation& animation, std::uint16_t frame, std::uint16_t x, std::uint16_t y, Pivot pivot = Pivot::TopLeft);

            void drawChar(char character, std::uint16_t x, std::uint16_t y, Pixel color, const Font& font);
            void drawText(const std::string& text, std::uint16_t x, std::uint16_t y, Pixel color);
            void drawText(const std::string& text, std::uint16_t x, std::uint16_t y, Pixel color, const Font& font);

            std::uint16_t measureText(const std::string& text, const Font& font) const;

            const Pixel* framebuffer() const;

        private:
            std::array<Pixel, ConsoleSpec::SCREEN_WIDTH * ConsoleSpec::SCREEN_HEIGHT> framebuffer_;
    };

} // namespace temgi
