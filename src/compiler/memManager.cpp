#include "memManager.h"

#include <cstdlib>
#include <stdexcept>

namespace sprout::memManager {

    void allocChunk(Memory& mem) {
        Chunk c {};
        c.mem = static_cast<uint8_t*>(std::malloc(CHUNK_SIZE));
        if (!c.mem) throw std::runtime_error("Faulty allocation!");

        c.size = CHUNK_SIZE;
        c.used = 0;

        mem.chunks.push_back(c);
    }

    void* allocateMemory(Memory& mem, const size_t size) {
        size_t total = align(size);
        if (mem.chunks.empty() || mem.chunks.back().used + total > CHUNK_SIZE) {
            allocChunk(mem);
        }
        Chunk& c = mem.chunks.back();

        void* ptr = (c.mem + c.used);

        c.used += total;
        mem.allocated += total;

        return ptr;
    }

    void freeMemory(Memory& mem) {
        for (const Chunk& c : mem.chunks) {
            std::free(c.mem);
        }
        mem.chunks.erase(mem.chunks.begin(), mem.chunks.end());
        mem.allocated = 0;
    }

}
