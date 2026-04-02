#ifndef SPROUT_LANG_VM_H
#define SPROUT_LANG_VM_H
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <bytecode_format.h>

constexpr uint16_t FUNCTION_TABLE_SIZE = 2048;

namespace sprout::heap {
    struct HEAP;
}

namespace sprout::vm {
    struct functionInfo {
        uint32_t entryIP;
        uint16_t frameSize;
    };

    struct VM {
        BCHeader header;
        uint64_t reg[256];
        std::vector<uint8_t> bytecode;
        std::size_t ip;

        uint64_t stack[4096];
        uint64_t sp;
        uint64_t fp;

        heap::HEAP* heapA;
        heap::HEAP* heapB;

        int8_t jmpFlag;

        bool running;
        bool heapAUsed;
        functionInfo functionTable[FUNCTION_TABLE_SIZE];
    };

    inline uint32_t fetch(VM& vm) {
        //DEBUG ONLY FEATURE if (vm.ip >= vm.bytecode.size()) { throw std::runtime_error("Instruction pointer out of bounds"); }
        uint8_t* p = vm.bytecode.data() + vm.ip;
        uint32_t a = (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
        vm.ip += 4;
        return a;
    }

    inline uint32_t fetchAtAddr(VM& vm, uint32_t addr) {
        if (addr >= vm.bytecode.size()) { throw std::runtime_error("Instruction pointer out of bounds"); }
        uint8_t* p = vm.bytecode.data() + addr;
        uint32_t a = (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
        return a;
    }

    inline functionInfo fetchFuncMetadata(VM& vm, uint32_t addr) {
        functionInfo f{};
        f.entryIP =  fetchAtAddr(vm, addr);
        f.frameSize = fetchAtAddr(vm, addr + 4);
        return f;
    }

    inline uint64_t fetch64(VM& vm) {
        return (static_cast<uint64_t>(fetch(vm)) << 32) | fetch(vm);
    }

    void init(VM& vm, const char* path);
    void run(VM& vm);

}
#endif //SPROUT_LANG_VM_H