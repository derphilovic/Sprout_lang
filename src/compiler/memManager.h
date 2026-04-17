#pragma once
#include <cstdint>
#include <vector>

namespace sprout::memManager {
    constexpr uint64_t CHUNK_SIZE = 4 * 1024 * 1024;
    constexpr std::size_t ALIGN = 8;

    inline std::size_t align(std::size_t n) {
        return (n + (ALIGN - 1)) & ~(ALIGN - 1);
    }

    struct Chunk {
        uint8_t* mem;
        std::size_t used;
        std::size_t size;
    };

    struct Memory {
        std::vector<Chunk> chunks;
        std::size_t allocated;
        std::size_t max;
    };

    void* allocateMemory(Memory& mem, const size_t size);
    void freeMemory(Memory& mem);

}
