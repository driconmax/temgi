#include <array>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <string>
#include <vector>

#include <temgi/PixelFormat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fs = std::filesystem;

struct ATimgHeader {
    char magic[5] = {'T', 'E', 'M', 'G', 'I'};
    std::uint8_t pixelFormat = static_cast<std::uint8_t>(temgi::CONSOLE_PIXEL_FORMAT);
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint16_t frameCount = 0;
};

struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

struct FrameFile
{
    fs::path path;
    std::uint32_t frameNumber;
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

// Packs an 8-bit-per-channel color into RGB565 (5 red / 6 green / 5 blue bits).
std::uint16_t packRGB565(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
    return static_cast<std::uint16_t>(
        ((r >> 3) << 11) |
        ((g >> 2) << 5) |
        (b >> 3)
    );
}


int main(int argc, char const *argv[])
{
    if(argc != 3){
        std::cout << "Usage: temgi-atlas <input-folder> <output.atimg>\n";

        return 1;
    }

    const fs::path inputFolder = argv[1];
    const fs::path outputPath = argv[2];

    if (!fs::exists(inputFolder) ||
        !fs::is_directory(inputFolder))
    {
        std::cerr
            << "Input path is not a directory.\n";

        return 1;
    }

    std::vector<FrameFile> frames;

    std::regex framePattern(
        R"(^(.+)_([0-9]+)\.png$)",
        std::regex::icase
    );

    /*std::regex framePattern(
        R"(^(.+?)([0-9]+)\.png$)",
        std::regex::icase
    );*/

    std::string animationName;

    for (const auto& entry :
         fs::directory_iterator(inputFolder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string filename =
            entry.path().filename().string();

        std::smatch match;

        if (!std::regex_match(
                filename,
                match,
                framePattern))
        {
            continue;
        }

        std::string currentName = match[1].str();

        std::uint32_t frameNumber =
            static_cast<std::uint32_t>(
                std::stoul(match[2].str())
            );

        if (animationName.empty())
        {
            animationName = currentName;
        }
        else if (currentName != animationName)
        {
            std::cerr
                << "Frame has different animation name: "
                << filename
                << '\n';

            return 1;
        }

        frames.push_back(
        {
            entry.path(),
            frameNumber
        });
    }

    if (frames.empty())
    {
        std::cerr
            << "No animation frames found.\n";

        return 1;
    }

    std::sort(
        frames.begin(),
        frames.end(),
        [](const FrameFile& a, const FrameFile& b)
        {
            return a.frameNumber < b.frameNumber;
        }
    );


    for (std::size_t i = 1; i < frames.size(); ++i)
    {
        if (frames[i].frameNumber ==
            frames[i - 1].frameNumber)
        {
            std::cerr
                << "Duplicate frame number: "
                << frames[i].frameNumber
                << '\n';

            return 1;
        }
    }

    if (frames.size() > 65535)
    {
        std::cerr
            << "Too many animation frames.\n";

        return 1;
    }

    const auto palette = createPalette();
    constexpr std::size_t BYTES_PER_PIXEL =
        (temgi::CONSOLE_PIXEL_FORMAT == temgi::PixelFormat::RGB565) ? 2 : 1;

    int animationWidth = 0;
    int animationHeight = 0;

    std::vector<std::uint8_t> animationPixels;

    for (const FrameFile& frame : frames)
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        unsigned char* image =
            stbi_load(
                frame.path.string().c_str(),
                &width,
                &height,
                &channels,
                4
            );

        if (image == nullptr)
        {
            std::cerr
                << "Could not load frame: "
                << frame.path
                << '\n';

            return 1;
        }


        // First frame defines animation dimensions.
        if (animationWidth == 0)
        {
            animationWidth = width;
            animationHeight = height;

            if (width > 65535 ||
                height > 65535)
            {
                std::cerr
                    << "Animation frame is too large.\n";

                stbi_image_free(image);
                return 1;
            }

            const std::size_t totalPixels =
                static_cast<std::size_t>(width) *
                static_cast<std::size_t>(height) *
                frames.size();

            animationPixels.reserve(totalPixels * BYTES_PER_PIXEL);
        }
        else if (
            width != animationWidth ||
            height != animationHeight)
        {
            std::cerr
                << "Frame has different dimensions: "
                << frame.path.filename()
                << '\n'
                << "Expected: "
                << animationWidth
                << "x"
                << animationHeight
                << '\n'
                << "Got: "
                << width
                << "x"
                << height
                << '\n';

            stbi_image_free(image);
            return 1;
        }


        // -------------------------------------------------
        // CONVERT THIS FRAME
        // -------------------------------------------------

        const std::size_t pixelCount =
            static_cast<std::size_t>(width) *
            static_cast<std::size_t>(height);

        for (std::size_t i = 0; i < pixelCount; ++i)
        {
            const std::size_t sourceIndex = i * 4;

            std::uint8_t r =
                image[sourceIndex + 0];

            std::uint8_t g =
                image[sourceIndex + 1];

            std::uint8_t b =
                image[sourceIndex + 2];

            std::uint8_t a =
                image[sourceIndex + 3];

            if constexpr (temgi::CONSOLE_PIXEL_FORMAT == temgi::PixelFormat::RGB565)
            {
                const std::uint16_t packed = (a == 0) ? 0x0000 : packRGB565(r, g, b);
                animationPixels.push_back(static_cast<std::uint8_t>(packed & 0xFF));
                animationPixels.push_back(static_cast<std::uint8_t>(packed >> 8));
            }
            else if (a == 0)
            {
                animationPixels.push_back(0x00);
            }
            else
            {
                animationPixels.push_back(
                    nearestColor(
                        r,
                        g,
                        b,
                        palette
                    )
                );
            }
        }

        stbi_image_free(image);

        std::cout
            << "Frame "
            << frame.frameNumber
            << ": "
            << frame.path.filename().string()
            << '\n';
    }

    ATimgHeader header;

    header.width =
        static_cast<std::uint16_t>(animationWidth);

    header.height =
        static_cast<std::uint16_t>(animationHeight);

    header.frameCount =
        static_cast<std::uint16_t>(frames.size());

    std::ofstream output(
        outputPath,
        std::ios::binary
    );

    if (!output)
    {
        std::cerr
            << "Could not create output file.\n";

        return 1;
    }

    output.write(
        header.magic,
        5
    );

    output.write(
        reinterpret_cast<const char*>(&header.pixelFormat),
        sizeof(header.pixelFormat)
    );

    output.write(
        reinterpret_cast<const char*>(&header.width),
        sizeof(header.width)
    );

    output.write(
        reinterpret_cast<const char*>(&header.height),
        sizeof(header.height)
    );

    output.write(
        reinterpret_cast<const char*>(&header.frameCount),
        sizeof(header.frameCount)
    );

    output.write(
        reinterpret_cast<const char*>(
            animationPixels.data()
        ),
        static_cast<std::streamsize>(
            animationPixels.size()
        )
    );

    if (!output)
    {
        std::cerr
            << "Error while writing ATIMG file.\n";

        return 1;
    }



    std::cout
        << "\nCreated "
        << outputPath
        << '\n'
        << "Animation: "
        << animationName
        << '\n'
        << "Frame size: "
        << animationWidth
        << "x"
        << animationHeight
        << '\n'
        << "Frames: "
        << frames.size()
        << '\n'
        << "Pixel data: "
        << animationPixels.size()
        << " bytes\n"
        << "Total size: "
        << 11 + animationPixels.size()
        << " bytes\n";

    return 0;
}
