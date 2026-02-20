#include "bytecode.h"

#include <cstring>
#include <fstream>
#include <vector>
#include <filesystem>

#include "vm.h"

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

    BCHeader loadHeader(const std::vector<uint8_t>& bytecode) {
        BCHeader subHeader{};
        if (bytecode.size() < sizeof(BCHeader)) {
            throw std::runtime_error("Bytecode too small to contain a header");
        }

        auto readU32 = [&](size_t offset) -> uint32_t {
            return (uint32_t(bytecode[offset])     << 24)
                 | (uint32_t(bytecode[offset + 1]) << 16)
                 | (uint32_t(bytecode[offset + 2]) << 8)
                 |  uint32_t(bytecode[offset + 3]);
        };

        subHeader.magic          = readU32(0);
        subHeader.version        = readU32(4);
        subHeader.flags          = readU32(8);
        subHeader.importCount    = readU32(12);
        subHeader.functionCount  = readU32(16);
        subHeader.importOffset   = readU32(20);
        subHeader.functionOffset = readU32(24);
        subHeader.codeOffset     = readU32(28);

        if (subHeader.magic != SPROUT_LANG_MAGIC) {
            throw std::runtime_error("Invalid bytecode file! SPRT Magic missing!");
        }
        if (subHeader.version != SPROUT_LANG_VERSION) {
            throw std::runtime_error("Unsupported bytecode version");
        }
        return subHeader;
    }

    std::vector<vm::functionInfo> loadFunctionTable(BCHeader header, vm::VM& vm) {
        std::vector<vm::functionInfo> f;
         uint32_t addr = sizeof(BCHeader);
        for (uint32_t i = 0; i < header.functionCount; ++i) {
            f.push_back(vm::fetchFuncMetadata(vm, addr));
            addr += sizeof(vm::functionInfo);
        }
        return f;
    }
}
