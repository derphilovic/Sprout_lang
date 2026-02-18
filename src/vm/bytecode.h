//
// Created by philipp on 10/02/2026.
//
#ifndef SPROUT_LANG_BYTECODE_H
#define SPROUT_LANG_BYTECODE_H

#define SPROUT_LANG_MAGIC 0x53505254
#define SPROUT_LANG_VERSION 0x00000001
#include <cstdint>
#include <vector>
#include "vm.h"

namespace sprout::bytecode {
    struct BCHeader {
        uint32_t magic;
        uint32_t version;
        uint32_t flags;

        uint32_t importCount;
        uint32_t functionCount;

        uint32_t importOffset;
        uint32_t functionOffset;
        uint32_t codeOffset;
    };


    std::vector<uint8_t> loadBytecode(const char* path);

    BCHeader loadHeader(const uint8_t* bytecode);

    class bytecode {
    };
}

#endif //SPROUT_LANG_BYTECODE_H