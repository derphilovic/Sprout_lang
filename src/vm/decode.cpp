#include "decode.h"
#include "vm.h"

namespace  sprout::decode {
    void push(vm::VM& vm, uint64_t a) {
        vm.stack[vm.sp++] = a;
    }

    uint64_t pop(vm::VM& vm) {
        return vm.stack[--vm.sp];
    }
}
