#ifndef SPROUT_LANG_VM_H
#define SPROUT_LANG_VM_H
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace sprout::vm {
    struct VM {
        uint64_t reg[256];
        std::vector<uint8_t> bytecode;
        std::size_t ip;

        int8_t jmpFlag;

        bool running;
    };

    inline uint32_t fetch(VM& vm) {
        if (vm.ip >= vm.bytecode.size()) { throw std::runtime_error("Instruction pointer out of bounds"); }
        uint8_t* p = vm.bytecode.data() + vm.ip;
        uint32_t a = (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
        vm.ip += 4;
        return a;
    }

    inline uint64_t fetch64(VM& vm) {
        return (static_cast<uint64_t>(fetch(vm)) << 32) | fetch(vm);
    }

    void init(VM& vm, const char* path);
    void run(VM& vm);

    class vm {
    };
}
#endif //SPROUT_LANG_VM_H