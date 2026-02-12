#include "vm.h"

#include "bytecode.h"
#include "decode.h"
#include "execution.h"


void init(VM& vm, const char* path) {
    vm.ip = 0;
    vm.bytecode = loadBytecode(path);
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