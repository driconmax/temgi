#include "temgi/internal/CartridgeLoader.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <dlfcn.h>

namespace temgi
{
	using CreateCartridgeFunction =
		CartridgeAPI* (*)();

	using DestroyCartridgeFunction =
		void (*)(CartridgeAPI*);

	struct CartridgeHeader
	{
		char magic[5];
		std::uint32_t version;
		std::uint32_t codeSize;
	};

    constexpr std::size_t CARTRIDGE_HEADER_SIZE = 25;

    constexpr std::size_t ASSET_ENTRY_NAME_SIZE = 64;

    constexpr std::size_t ASSET_ENTRY_SIZE =
        64 +    // name
        4 +     // offset
        4;      // size

	CartridgeLoader::CartridgeLoader()
	{
	}

	CartridgeLoader::~CartridgeLoader()
	{
		unload();
	}

	bool CartridgeLoader::load(const std::string& path)
	{
		unload();

		std::ifstream file(path, std::ios::binary);

		if (!file)
		{
			std::cerr << "Could not open cartridge.\n";
			return false;
		}

		file.seekg(0, std::ios::end);

		const std::size_t cartridgeSize =
			static_cast<std::size_t>(
				file.tellg()
			);

		file.seekg(0, std::ios::beg);

		cartridgeData_.resize(cartridgeSize);

		file.read(
			reinterpret_cast<char*>(
				cartridgeData_.data()
			),
			static_cast<std::streamsize>(
				cartridgeData_.size()
			)
		);

		if (!file)
		{
			std::cerr
				<< "Could not read cartridge ROM.\n";

			return false;
		}

		file.clear();
		file.seekg(0, std::ios::beg);

		char magic[5];

		std::uint32_t version = 0;
		std::uint32_t codeSize = 0;
		std::uint32_t assetCount = 0;
		std::uint32_t assetTableOffset = 0;
		std::uint32_t assetDataOffset = 0;

		file.read(
			magic,
			5
		);

		file.read(
			reinterpret_cast<char*>(&version),
			sizeof(version)
		);

		file.read(
			reinterpret_cast<char*>(&codeSize),
			sizeof(codeSize)
		);

		file.read(
			reinterpret_cast<char*>(&assetCount),
			sizeof(assetCount)
		);

		file.read(
			reinterpret_cast<char*>(&assetTableOffset),
			sizeof(assetTableOffset)
		);

		file.read(
			reinterpret_cast<char*>(&assetDataOffset),
			sizeof(assetDataOffset)
		);

		if (!file)
		{
			std::cerr << "Could not read cartridge \n";
			return false;
		}

		if (
			magic[0] != 'T' ||
			magic[1] != 'E' ||
			magic[2] != 'M' ||
			magic[3] != 'G' ||
			magic[4] != 'I'
		)
		{
			std::cerr << "Invalid TEMGI cartridge.\n";
			return false;
		}

		if (version != 1)
		{
			std::cerr
				<< "Unsupported TEMGI cartridge version: "
				<< version
				<< '\n';

			return false;
		}


		assetCount_ = assetCount;
		assetTableOffset_ = assetTableOffset;
		assetDataOffset_ = assetDataOffset;

		std::vector<char> code(codeSize);

		file.read(
			code.data(),
			static_cast<std::streamsize>(code.size())
		);

		if (!file)
		{
			std::cerr << "Could not read cartridge code.\n";
			return false;
		}
		
		auto temporaryPath =
			std::filesystem::temp_directory_path() /
			"temgi_cartridge.dylib";

		extractedLibraryPath_ =
			temporaryPath.string();

		std::ofstream extracted(
			extractedLibraryPath_,
			std::ios::binary
		);

		if (!extracted)
		{
			std::cerr
				<< "Could not extract cartridge library.\n";

			return false;
		}

		extracted.write(
			code.data(),
			static_cast<std::streamsize>(code.size())
		);

		extracted.close();

		libraryHandle_ = dlopen(
			extractedLibraryPath_.c_str(),
			RTLD_LAZY
		);

		if (libraryHandle_ == nullptr)
		{
			std::cerr
				<< "Could not load cartridge library: "
				<< dlerror()
				<< '\n';

			unload();
			return false;
		}

		void* createSymbol =
			dlsym(
				libraryHandle_,
				"temgiCreateCartridge"
			);

		if (createSymbol == nullptr)
		{
			std::cerr
				<< "Missing function: "
				<< "temgiCreateCartridge\n";

			unload();
			return false;
		}

		auto createCartridge =
			reinterpret_cast<CreateCartridgeFunction>(
				createSymbol
			);

		cartridge_ = createCartridge();

		if (cartridge_ == nullptr)
		{
			std::cerr
				<< "temgiCreateCartridge returned nullptr.\n";

			unload();
			return false;
		}

		return true;
	}

	void CartridgeLoader::unload()
	{
		if (
			cartridge_ != nullptr &&
			libraryHandle_ != nullptr
		)
		{
			void* destroySymbol =
				dlsym(
					libraryHandle_,
					"temgiDestroyCartridge"
				);

			if (destroySymbol != nullptr)
			{
				auto destroyCartridge =
					reinterpret_cast<DestroyCartridgeFunction>(
						destroySymbol
					);

				destroyCartridge(cartridge_);
			}

			cartridge_ = nullptr;
		}

		if (libraryHandle_ != nullptr)
		{
			dlclose(libraryHandle_);
			libraryHandle_ = nullptr;
		}

		if (!extractedLibraryPath_.empty())
		{
			std::filesystem::remove(
				extractedLibraryPath_
			);

			extractedLibraryPath_.clear();
		}
	}

	CartridgeAPI* CartridgeLoader::cartridge()
	{
		return cartridge_;
	}

    Asset CartridgeLoader::asset(const std::string &name) const
    {
        for (std::uint32_t i = 0; i < assetCount_; i++)
		{
			const std::size_t entryPosition = static_cast<std::size_t>(assetTableOffset_) + static_cast<std::size_t>(i) * ASSET_ENTRY_SIZE;

			const std::uint8_t* entry = cartridgeData_.data() + entryPosition;
			const char* assetName = reinterpret_cast<const char*>(entry);

			std::uint32_t offset = 0;
			std::uint32_t size = 0;

			std::memcpy(&offset, entry + ASSET_ENTRY_NAME_SIZE, sizeof(offset));
			std::memcpy(&size, entry + ASSET_ENTRY_NAME_SIZE + sizeof(offset), sizeof(size));

			if(name == assetName){
				const std::size_t dataPosition = static_cast<std::size_t>(assetDataOffset_) + static_cast<std::size_t>(offset);

				return Asset{cartridgeData_.data() + dataPosition,  size};
			}
		}

		return Asset{};
		
    }
}