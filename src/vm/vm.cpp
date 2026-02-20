#include "vm.h"

#include "bytecode.h"
#include "bytecode.h"
#include "decode.h"
#include "execution.h"

namespace sprout::vm {
    void init(VM& vm, const char* path) {
        vm.bytecode = bytecode::loadBytecode(path);
        vm.header = bytecode::loadHeader(vm.bytecode);
        vm.functionTable = bytecode::loadFunctionTable(vm.header, vm);
        vm.ip = vm.header.codeOffset;
        vm.jmpFlag = 2;
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