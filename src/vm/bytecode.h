//
// Created by philipp on 10/02/2026.
//
#ifndef SPROUT_LANG_BYTECODE_H
#define SPROUT_LANG_BYTECODE_H

#define SPROUT_LANG_MAGIC 0x53505254
#define SPROUT_LANG_VERSION 0x00000001
#include <cstdint>
#include <vector>

namespace sprout::vm {
    struct functionInfo;
    struct VM;
}

namespace sprout::bytecode {
    struct BCHeader {
        uint32_t magic; //Magic that spells SPRT
        uint32_t version;
        uint32_t flags;

        uint32_t importCount; // Count of modules to Import
        uint32_t functionCount; // Count of functions to register

        uint32_t importOffset;
        uint32_t functionOffset;
        uint32_t codeOffset;
    };


    std::vector<uint8_t> loadBytecode(const char* path);

    std::vector<vm::functionInfo> loadFunctionTable(BCHeader header, vm::VM& vm);

    BCHeader loadHeader(const std::vector<uint8_t>& bytecode);

}

#endif //SPROUT_LANG_BYTECODE_H