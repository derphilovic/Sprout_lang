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
        uint64_t forwarded;
    };

    enum objType {
        OBJ_STRING,
        OBJ_ARRAY,
    };

    enum flags : uint16_t {
        FLAG_UNMARKED,
        FLAG_MARKED
    };

    struct arrayObj {
    objHeader header;
    uint32_t type;
    uint32_t length;
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


    void compactingGarbageCollect(vm::VM& vm);
    void freeHeap(HEAP& h);
    void* heapAlloc (HEAP& h, size_t size, uint16_t type);
    void* gcCollectedHeapAlloc (size_t size, uint16_t type, vm::VM& vm);
    void initArray(vm::VM& vm, uint8_t dst, uint8_t len, uint8_t type);
    void moveArray(vm::VM& vm,  uint8_t target, uint8_t address, uint8_t index, uint8_t flag);

}
#endif