#include "CartridgeLoader.h"

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

		CartridgeHeader header{};

		file.read(
			reinterpret_cast<char*>(&header),
			sizeof(header)
		);

		if (!file)
		{
			std::cerr << "Could not read cartridge header.\n";
			return false;
		}

		if (
			header.magic[0] != 'T' ||
			header.magic[1] != 'E' ||
			header.magic[2] != 'M' ||
			header.magic[3] != 'G' ||
			header.magic[4] != 'I'
		)
		{
			std::cerr << "Invalid TEMGI cartridge.\n";
			return false;
		}

		if (header.version != 1)
		{
			std::cerr
				<< "Unsupported TEMGI cartridge version: "
				<< header.version
				<< '\n';

			return false;
		}

		std::vector<char> code(header.codeSize);

		file.read(
			code.data(),
			static_cast<std::streamsize>(code.size())
		);

		if (!file)
		{
			std::cerr << "Could not read cartridge code.\n";
			return false;
		}

		/*
		 * dlopen() needs an actual dynamic library file.
		 * So for the desktop simulator we temporarily extract
		 * the compiled cartridge code.
		 */
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
}