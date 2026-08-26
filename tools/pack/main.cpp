#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

struct CartridgeHeader {
    char magic[5] = {'T', 'E', 'M', 'G', 'I'};
    std::uint32_t version = 1;
    std::uint32_t codeSize = 0;
};

int main(int argc, char const *argv[])
{
    if(argc != 3){
        std::cout << "Usage: temgi-pack <input> <output.temgi>\n";
        return 1;
    }

    const char* inputPath = argv[1];
    const char* outputPath = argv[2];

    std::ifstream input(inputPath, std::ios::binary);

    if(!input){
        std::cerr << "Could not open the input file.\n";
        return 1;
    }

    input.seekg(0, std::ios::end);
    const std::size_t size = static_cast<std::size_t>(input.tellg());
    input.seekg(0, std::ios::beg);

    std::vector<char> code(size);
    input.read(code.data(), size);

    CartridgeHeader header;
    header.codeSize = static_cast<std::uint32_t>(size);

    std::ofstream output(outputPath, std::ios::binary);

    if(!output){
        std::cerr << "Could not create output file.\n";
        return 1;
    }

    output.write(reinterpret_cast<const char*>(&header), sizeof(header));
    output.write(code.data(), code.size());

    std::cout << "Created " << outputPath << " - " << size << " bytes";

    return 0;
}
