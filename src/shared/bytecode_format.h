#pragma once
#include <cstdint>

constexpr uint32_t SPROUT_LANG_MAGIC = 0x53505254;
constexpr uint32_t SPROUT_LANG_VERSION = 0x00000001;

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