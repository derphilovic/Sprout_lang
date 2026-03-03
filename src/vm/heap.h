#ifndef SPROUT_LANG_HEAP_H
#define SPROUT_LANG_HEAP_H
#include <cstdint>
#include <vector>

#include "decode.h"

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

    enum flags : uint16_t {
        FLAG_UNMARKED,
        FLAG_MARKED
    };

    struct CHUNK {
        uint8_t* mem;
        size_t used;
        size_t capacity;
    };

    struct HEAP {
        std::vector<CHUNK> chunks;
        size_t totalAllocated;
        size_t max;
    };

    inline bool markObject(uint64_t& r) {
        if (decode::isPointer(r)) {
            auto* hdr = static_cast<objHeader*>(decode::decodePointer(r)) - 1; // step back to header
            hdr->flags |= FLAG_MARKED;
            return true;
        }
        return false;
    }

    void compactingGarbageCollect(vm::VM& vm);
    void freeHeap(HEAP& h);
    void* heapAlloc (HEAP& h, size_t size, uint16_t type);
    void* gcCollectedHeapAlloc (HEAP& h, size_t size, uint16_t type, vm::VM& vm);

}
#endif