#include "heap.h"
#include <cstdlib>

namespace sprout::heap {

    void allocNewChunk(heap& Heap) {
            chunk c {};
            c.mem = static_cast<uint8_t *>(std::malloc(CHUNK_SIZE));
            if (!c.mem) {
                throw std::bad_alloc{};
            }
            c.capacity = CHUNK_SIZE;
            c.used = 0;

            Heap.chunks.push_back(c);
        }

    void* heapAlloc (heap& h, size_t size) {
        size = align(size);
        if (h.chunks.empty() || h.chunks.back().used + size > CHUNK_SIZE) {
            allocNewChunk(h);
        }

        chunk& c = h.chunks.back();
        void* ptr = c.mem + c.used;
        c.used += size;

        h.totalAllocated += size;

        return ptr;
    }

    void freeHeap(heap& h) {
        for (auto& c : h.chunks) {
            std::free(c.mem);
        }
    }

}
