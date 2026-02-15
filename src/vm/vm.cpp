#include "vm.h"

#include "bytecode.h"
#include "decode.h"
#include "execution.h"

namespace sprout::vm {
    void init(VM& vm, const char* path) {
        vm.ip = 0;
        vm.jmpFlag = 2;
        vm.bytecode = bytecode::loadBytecode(path);
        vm.running = true;
    }

    void run(VM& vm) {
        while (vm.running) {
            uint32_t instruction = fetch(vm);
            decode::decodedInstr d = decode::decode(instruction);
            execution::execute(vm, d);
        }
    }
}