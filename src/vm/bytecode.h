//
// Created by philipp on 10/02/2026.
//
#ifndef SPROUT_LANG_BYTECODE_H
#define SPROUT_LANG_BYTECODE_H

#include <cstdint>
#include <vector>
#include <bytecode_format.h>

namespace sprout::vm {
    struct functionInfo;
    struct VM;
}

namespace sprout::bytecode {


    std::vector<uint8_t> loadBytecode(const char* path);

    BCHeader loadHeader(const std::vector<uint8_t>& bytecode);

}

#endif //SPROUT_LANG_BYTECODE_H