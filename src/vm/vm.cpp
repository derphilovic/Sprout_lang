#include "vm.h"

#include "bytecode.h"
#include "decode.h"
#include "execution.h"

uint32_t fetch(VM& vm) {
    uint8_t* p = vm.bytecode + vm.ip;
    uint32_t a = (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
    vm.ip += 4;
    return a;
}

void init(VM& vm, uint8_t* bytecode) {
    vm.ip = 0;
    vm.bytecode = bytecode;
    vm.bytecode_size = sizeof(vm.bytecode);
    vm.running = true;
}

void run(VM& vm) {
    while (vm.running) {
        uint32_t instruction = fetch(vm);
        decodedInstr d = decode(instruction);
        execute(vm, d);
    }
}