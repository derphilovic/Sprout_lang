#include "bytecode.h"

#include <cstring>
#include <fstream>
#include <vector>
#include <filesystem>

namespace sprout::bytecode {
    std::vector<std::byte> loadRawBytecode(const char* path)
    {
        std::ifstream inputFile(path, std::ios::binary | std::ios::ate);
        if (!inputFile)
            return {};
        auto size = inputFile.tellg();
        if (size <= 0)
            return {};

        inputFile.seekg(0, std::ios::beg);
        std::vector<std::byte> buffer(static_cast<size_t>(size));
        inputFile.read(reinterpret_cast<char*>(buffer.data()), size);
        return buffer;
    }


    std::vector<uint8_t> loadBytecode(const char* path) {
        auto raw = loadRawBytecode(path);
        std::vector<uint8_t> vec(raw.size());
        std::memcpy(vec.data(), raw.data(), raw.size());
        return vec;
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
}