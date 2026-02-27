#ifndef SPROUT_LANG_HEAP_H
#define SPROUT_LANG_HEAP_H
#include <cstdint>
#include <vector>

namespace sprout::heap {
    constexpr size_t CHUNK_SIZE = 4 * 1024 * 1024;
    constexpr size_t ALIGN = 8;

    inline size_t align(size_t n) {
        return (n + (ALIGN - 1)) & ~(ALIGN - 1);
    }

    struct objHeader {
        uint32_t size;
        uint16_t type;
        uint16_t flags;
    };

    enum objType {
        OBJ_STRING,
        OBJ_ARRAY,
    };

    struct chunk {
        uint8_t* mem;
        size_t used;
        size_t capacity;
    };

    struct heap {
        std::vector<chunk> chunks;
        size_t totalAllocated;
        size_t nextGC;
    };

}
#endif