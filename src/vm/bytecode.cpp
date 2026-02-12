#include "bytecode.h"

#include <cstring>
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::byte> loadRawBytecode(const char* path) {
    std::filesystem::path inputFilePath{path};
    auto length = std::filesystem::file_size(inputFilePath);
    if (length == 0) {
        return {};  // empty vector
    }
    std::vector<std::byte> buffer(length);
    std::ifstream inputFile(path, std::ios_base::binary);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
    inputFile.close();
    return buffer;
}

uint8_t* loadBytecode(const char* path) {
    std::vector<std::byte> raw = loadRawBytecode(path);
    return reinterpret_cast<uint8_t*>(raw.data());
}

BCHeader loadHeader(const uint8_t* bytecode) {
    BCHeader header{};
    std::memcpy(&header, bytecode, sizeof(BCHeader));
    if (header.magic != SPROUT_LANG_MAGIC) {
        throw std::runtime_error("Invalid bytecode file");
    }
    if (header.version != SPROUT_LANG_VERSION) {
        throw std::runtime_error("Unsupported bytecode version");
    }
    return header;
}