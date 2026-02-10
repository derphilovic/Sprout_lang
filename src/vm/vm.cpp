#include "vm.h"

#include "bytecode.h"
#include "decode.h"
#include "execution.h"

uint32_t fetch(VM& vm) {
    uint32_t a = *(uint32_t*)(vm.bytecode + vm.ip);
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