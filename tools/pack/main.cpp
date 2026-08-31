#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <cstring>
#include <string>

#include <temgi/PixelFormat.h>

namespace fs = std::filesystem;

struct CartridgeHeader {
	char magic[5] = {'T', 'E', 'M', 'G', 'I'};
	std::uint32_t version = 2;
	std::uint8_t pixelFormat = static_cast<std::uint8_t>(temgi::CONSOLE_PIXEL_FORMAT);
	std::uint32_t codeSize = 0;
	std::uint32_t assetCount = 0;
	std::uint32_t assetTableOffset = 0;
	std::uint32_t assetDataOffset = 0;
};

struct AssetEntry
{
	char name[64];

	std::uint32_t offset;
	std::uint32_t size;
};

struct PackedAsset
{
	AssetEntry entry;
	std::vector<char> data;
};

constexpr std::uint32_t CARTRIDGE_HEADER_SIZE = 
	5 +     // MAGIC
	4 +     // Version
	1 +     // Pixel format
	4 +     // Code Size
	4 +     // Asset Count
	4 +     // Asset Table Offset
	4       // Asset Data Offset
;

constexpr std::uint32_t ASSET_ENTRY_SIZE = 
	64 +	// Name
	4 + 	// Offset
	4;		// Size

int main(int argc, char const *argv[])
{
	if(argc != 4){
		std::cout << "Usage: temgi-pack <input> <assets-folder> <output.temgi>\n";
		return 1;
	}

	const char* inputPath = argv[1];
	const char* assetsPath = argv[2];
	const char* outputPath = argv[3];

	std::ifstream input(inputPath, std::ios::binary);

	if(!input){
		std::cerr << "Could not open the input file.\n";
		return 1;
	}

	input.seekg(0, std::ios::end);
	const std::size_t codeSize = static_cast<std::size_t>(input.tellg());
	input.seekg(0, std::ios::beg);

	std::vector<char> code(codeSize);
	input.read(code.data(), codeSize);

	if (codeSize > UINT32_MAX)
	{
		std::cerr << "Game code is too large.\n";
		return 1;
	}

	CartridgeHeader header;
	header.codeSize = static_cast<std::uint32_t>(codeSize);


	if(!fs::exists(assetsPath) || !fs::is_directory(assetsPath)){
		std::cerr << "Could not open the input file.\n";
		return 1;
	} 


	std::vector<PackedAsset> assets;
	for(const auto& file : fs::recursive_directory_iterator(assetsPath)){
		if(!file.is_regular_file()){
			continue;
		}

		std::ifstream assetFile(file.path(), std::ios::binary);

		if(!assetFile){
			std::cerr << "Could not open asset: " << file.path() << '\n';

			return 1;
		}

		assetFile.seekg(0, std::ios::end);
		
		const std::size_t assetSize = static_cast<std::size_t>(assetFile.tellg());

		assetFile.seekg(0, std::ios::beg);

		if(assetSize > UINT32_MAX){
			std::cerr << "Asset is too large" << file.path() << '\n';
			return 1;
		}

		PackedAsset asset{};

		asset.data.resize(assetSize);

		assetFile.read(asset.data.data(), static_cast<std::streamsize>(asset.data.size()));

		if(!assetFile){
			std::cerr << "Could not read asset: " << file.path() << '\n';
			return 1;
		}

		std::string assetName = fs::relative(file.path(), assetsPath).generic_string();

		if(assetName.size() >= sizeof(asset.entry.name)){
			std::cerr << "Asset name is too long: " << assetName << '\n';
			return 1;
		}

		std::memcpy(asset.entry.name, assetName.c_str(), assetName.size());

		asset.entry.size = static_cast<std::uint32_t>(assetSize);

		assets.push_back(std::move(asset));
	}

	header.assetCount = static_cast<std::uint32_t>(assets.size());
	header.assetTableOffset = CARTRIDGE_HEADER_SIZE + header.codeSize;
	header.assetDataOffset = header.assetTableOffset + header.assetCount * ASSET_ENTRY_SIZE;

	std::uint32_t currentAssetOffset = 0;

	for(PackedAsset& asset : assets){
		asset.entry.offset = currentAssetOffset;
		currentAssetOffset += asset.entry.size;
	}


	std::ofstream output(outputPath, std::ios::binary);

	if(!output){
		std::cerr << "Could not create output file.\n";
		return 1;
	}

    output.write(
        header.magic,
        5
    );


    output.write(
        reinterpret_cast<const char*>(
            &header.version
        ),
        sizeof(header.version)
    );

    output.write(
        reinterpret_cast<const char*>(
            &header.pixelFormat
        ),
        sizeof(header.pixelFormat)
    );


    output.write(
        reinterpret_cast<const char*>(
            &header.codeSize
        ),
        sizeof(header.codeSize)
    );


    output.write(
        reinterpret_cast<const char*>(
            &header.assetCount
        ),
        sizeof(header.assetCount)
    );


    output.write(
        reinterpret_cast<const char*>(
            &header.assetTableOffset
        ),
        sizeof(header.assetTableOffset)
    );


    output.write(
        reinterpret_cast<const char*>(
            &header.assetDataOffset
        ),
        sizeof(header.assetDataOffset)
    );

	output.write(code.data(), code.size());


    for (const PackedAsset& asset : assets)
    {
        output.write(asset.entry.name, sizeof(asset.entry.name));
        output.write(reinterpret_cast<const char*>(&asset.entry.offset), sizeof(asset.entry.offset));
        output.write(reinterpret_cast<const char*>(&asset.entry.size), sizeof(asset.entry.size));
    }

	for (const PackedAsset& asset : assets)
	{
		output.write(
			asset.data.data(),
			static_cast<std::streamsize>(
				asset.data.size()
			)
		);
	}

	if (!output)
	{
		std::cerr
			<< "Error while writing cartridge.\n";

		return 1;
	}


    std::cout
        << "Created "
        << outputPath
        << '\n'
        << '\n'
        << "Code: "
        << codeSize
        << " bytes\n"
        << "Assets: "
        << assets.size()
        << '\n';


    for (const PackedAsset& asset : assets)
    {
        std::cout
            << "  "
            << asset.entry.name
            << " - "
            << asset.entry.size
            << " bytes\n";
    }


    const std::size_t totalSize =
        header.assetDataOffset +
        currentAssetOffset;


    std::cout
        << '\n'
        << "Total cartridge size: "
        << totalSize
        << " bytes\n";


    return 0;
}
