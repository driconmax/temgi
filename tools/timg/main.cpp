#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct TimgHeader {
    char magic[5] = {'T', 'E', 'M', 'G', 'I'};
    std::uint16_t width = 0;
    std::uint16_t height = 0;
};

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

std::array<Color, 256> createPalette() {
    std::array<Color, 256> palette{};

    palette[0] = {0, 0, 0};

    std::size_t index = 1;

    constexpr std::uint8_t levels[6] = {
        0,
        51,
        102,
        153,
        204,
        255
    };

    // 216 COLORS
    for(std::uint8_t r : levels){
        for(std::uint8_t g : levels){
            for(std::uint8_t b : levels){
                palette[index++] = {r, g, b};
            }
        }
    }

    // 39 GRAYSCALE
    constexpr std::size_t grayCount = 39;

    for (std::size_t i = 0; i < grayCount; i++)
    {
        std::uint8_t value = static_cast<std::uint8_t>((i * 255) / (grayCount - 1));

        palette[index++] = {
            value,
            value,
            value
        };
    }

    return palette;
}

std::uint8_t nearestColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, const std::array<Color, 256>& palette){

    std::uint32_t bestDistance =
        std::numeric_limits<std::uint32_t>::max();

    std::uint8_t bestIndex = 1;

    // Start at 1 because 0 means transparent.
    for (std::size_t i = 1; i < palette.size(); ++i)
    {
        int dr =
            static_cast<int>(r) -
            static_cast<int>(palette[i].r);

        int dg =
            static_cast<int>(g) -
            static_cast<int>(palette[i].g);

        int db =
            static_cast<int>(b) -
            static_cast<int>(palette[i].b);

        std::uint32_t distance =
            static_cast<std::uint32_t>(
                dr * dr +
                dg * dg +
                db * db
            );

        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestIndex =
                static_cast<std::uint8_t>(i);
        }
    }

    return bestIndex;
}


int main(int argc, char const *argv[])
{
    if(argc != 3){
        std::cout << "Usage: temgi-image <input.png> <output.timg>\n";
    }

    const char* inputPath = argv[1];
    const char* outputPath = argv[2];

    int width = 0;
    int height = 0;
    int channels = 0;

    unsigned char* image = stbi_load(inputPath, &width, &height, &channels, 4);

    if(image == nullptr){
        std::cerr << "Could not open the input file.\n";
        return 1;
    }
    
    if(width > 65535 || height > 65535){
        std::cerr << "Image is too large fo TIMG.\n";

        stbi_image_free(image);
        return 1;
    }

    const auto palette = createPalette();
    std::vector<std::uint8_t> pixels(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            std::size_t sourceIndex = (static_cast<std::size_t>(y) * width + x) * 4;

            std::uint8_t r = image[sourceIndex + 0];
            std::uint8_t g = image[sourceIndex + 1];
            std::uint8_t b = image[sourceIndex + 2];
            std::uint8_t a = image[sourceIndex + 3];

            std::size_t destinationIndex = (static_cast<std::size_t>(y) * width + x);

            if(a == 0){
                pixels[destinationIndex] = 0x00;
            } else {
                pixels[destinationIndex] = nearestColor(r, g, b, palette);
            }
        }
    }

    stbi_image_free(image);

    TimgHeader header;
    header.width = static_cast<std::uint16_t>(width);
    header.height = static_cast<std::uint16_t>(height);

    std::ofstream output(outputPath, std::ios::binary);

    if(!output){
        std::cerr << "Could not create output file.\n";
    }

    output.write(reinterpret_cast<const char*>(&header), sizeof(header));
    output.write(reinterpret_cast<const char*>(pixels.data()), static_cast<std::streamsize>(pixels.size()));

    std::cout
        << "Created " << outputPath << '\n'
        << "Size: " << width << "x" << height << '\n'
        << "Pixels: " << pixels.size() << " bytes\n";

    return 0;
}
