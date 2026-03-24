#include "decode.h"
#include "vm.h"

namespace  sprout::decode {
    void push(vm::VM& vm, uint64_t a) {
        if (vm.sp >= 4096) throw std::runtime_error("!!!STACK OVERFLOW!!!");
        vm.stack[vm.sp++] = a;
    }

    uint64_t pop(vm::VM& vm) {
        return vm.stack[--vm.sp];
    }
}
