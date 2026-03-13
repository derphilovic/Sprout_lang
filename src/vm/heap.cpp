#include "heap.h"
#include <cstdlib>

#include "decode.h"
#include "vm.h"

namespace sprout::heap {

    inline bool markObject(uint64_t& r) {
        if (decode::isPointer(r)) {
            auto* hdr = static_cast<objHeader*>(decode::decodePointer(r)) - 1; // step back to header
            hdr->flags |= FLAG_MARKED;
            return true;
        }
        return false;
    }

    void allocNewChunk(HEAP& Heap) {
            CHUNK c {};
            c.mem = static_cast<uint8_t *>(std::malloc(CHUNK_SIZE));
            if (!c.mem) {
                throw std::bad_alloc{};
            }
            c.capacity = CHUNK_SIZE;
            c.used = 0;

            Heap.chunks.push_back(c);
        }

    void* heapAlloc (HEAP& h, size_t size, uint16_t type) {
        size_t total = align(sizeof(objHeader) + size);
        if (h.chunks.empty() || h.chunks.back().used + total > CHUNK_SIZE) {
            allocNewChunk(h);
        }
        CHUNK& c = h.chunks.back();
        void* ptr = c.mem + c.used;

        objHeader* hdr = reinterpret_cast<objHeader*>(ptr);
        hdr->size = total;
        hdr->type = type;
        hdr->flags = 0;
        hdr->forwarded = reinterpret_cast<uint64_t>(nullptr);

        c.used += total;
        h.totalAllocated += total;

        return hdr + 1;
    }

    void* gcCollectedHeapAlloc (size_t size, uint16_t type, vm::VM& vm) {
        HEAP& active = vm.heapAUsed ? *vm.heapA : *vm.heapB;
        if (active.totalAllocated > active.max * 0.7) {
            compactingGarbageCollect(vm);
            active = vm.heapAUsed ? *vm.heapA : *vm.heapB; //Reevaluate because active is switched in GC
        }
        return heapAlloc(active, size, type);
        }

    void freeHeap(HEAP& h) {
        for (auto& c : h.chunks) {
            std::free(c.mem);
        }
        h.chunks.erase(h.chunks.begin(), h.chunks.end());
        h.totalAllocated = 0;
    }

    void freeChunk(CHUNK& c) {
        std::free(c.mem);
        c.mem = nullptr;
    }

    std::vector<objHeader*> getAllObjects(HEAP& h) {
        std::vector<objHeader*> objects;
        for (auto& c : h.chunks) {
            uint32_t i = 0;
            while (i < c.used) {
                auto ptr = reinterpret_cast<objHeader*>(c.mem + i);
                objects.push_back(ptr);
                i += ptr->size;
            }
        }
        return objects;
    }

    std::vector<uint64_t*> markObjects(vm::VM& vm) {
        HEAP*& h = vm.heapAUsed ? vm.heapA : vm.heapB;
        std::vector<objHeader*> objects = getAllObjects(*h);
        std::vector<uint64_t*> ptrHolder;
        for (auto& r : vm.reg) {
            if (markObject(r)) ptrHolder.push_back(&r);
        }
        for (uint64_t i = 0; i < vm.sp; i++) {
            if (markObject(vm.stack[i])) ptrHolder.push_back(&vm.stack[i]);
        }
        for (auto object : objects) {
            if (object->type == OBJ_ARRAY) {
                auto* array = reinterpret_cast<arrayObj *>(object);
                if (array->type == decode::TAG_POINTER) {
                    auto* data = reinterpret_cast<uint64_t*>(
                        reinterpret_cast<uint8_t*>(array) + sizeof(arrayObj));
                    for (uint32_t i = 0; i < array->length; i++) {
                        if (markObject(data[i])) ptrHolder.push_back(&data[i]);
                    }
                }
            }
        }
        return ptrHolder;
    }

    void moveObjects(HEAP& h1, HEAP& h2) {
        for (auto& c : h1.chunks) {
            uint32_t i = 0;

            while (i < c.used) {
                auto* obj = reinterpret_cast<objHeader*>(c.mem + i);

                if (obj->flags == FLAG_MARKED) {
                    auto* newAddr = static_cast<objHeader*>(heapAlloc(h2, obj->size, obj->type)); //get a new * address for obj from heapAlloc

                    std::memcpy(newAddr, obj + 1, obj->size - sizeof(objHeader)); //copy data from the old obj to the new obj

                    obj->forwarded = reinterpret_cast<uint64_t>(newAddr);
                }
                i += obj->size;
            }
        }
    }

    void updatePtrHolder(HEAP& h, std::vector<uint64_t*>& ptrHolder) {
        for (auto& c : ptrHolder) {
            void* raw = decode::decodePointer(*c);
            auto* hdr = static_cast<objHeader*>(raw) - 1;
            *c = decode::encodePointer(hdr->forwarded);

        }
        freeHeap(h);
    }

    void compactingGarbageCollect(vm::VM& vm) {
            std::vector<uint64_t*> ptrHolder = markObjects(vm); //Get Stack slots or regs with heap pointers

            if (vm.heapAUsed) {
                moveObjects(*vm.heapA, *vm.heapB); //Compact heapA into heapB
                updatePtrHolder(*vm.heapA, ptrHolder); //Patch roots, free old heap
                vm.heapAUsed = false;
            } else {
                moveObjects(*vm.heapB, *vm.heapA); //Compact heapB into heapA
                updatePtrHolder(*vm.heapB, ptrHolder); //Patch roots, free old heap
                vm.heapAUsed = true;
            }
    }

    void initArray(vm::VM& vm, uint8_t dst, uint8_t len, uint8_t type) {
        HEAP*& h = vm.heapAUsed ? vm.heapA : vm.heapB;
        auto* ptr = static_cast<arrayObj*>(gcCollectedHeapAlloc(vm.reg[len] + (sizeof(uint32_t) * 2), OBJ_ARRAY, vm)) - 1;
        ptr->length = vm.reg[len];
        ptr->type = vm.reg[type];
        std::memset(ptr + sizeof(arrayObj), 0, ptr->length * sizeof(uint64_t));

        vm.reg[dst] = decode::encodePointer(reinterpret_cast<uint64_t>(ptr));
    }


};