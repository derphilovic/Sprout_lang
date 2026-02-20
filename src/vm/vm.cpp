#include "vm.h"
#include <format>
#include "bytecode.h"
#include "decode.h"
#include "execution.h"

namespace sprout::vm {
    void init(VM& vm, const char* path) {
        vm.bytecode = bytecode::loadBytecode(path);
        vm.header = bytecode::loadHeader(vm.bytecode);
        vm.functionTable = bytecode::loadFunctionTable(vm.header, vm);
        vm.ip = vm.header.codeOffset;
        // Debug print
        /*std::cerr << "codeOffset = " << vm.header.codeOffset
          << ", sizeof(BCHeader) = " << sizeof(bytecode::BCHeader) 
          << ", ip = " << vm.ip << std::endl;*/
        vm.jmpFlag = 2;
        vm.running = true;
    }

    void run(VM& vm) {
        while (vm.running) {
            //std::cerr << "ip=" << vm.ip << " "; //DEBUG FUNC
            uint32_t instruction = fetch(vm);
            //std::cerr << std::format("instr=0x{:08X}", instruction) << std::endl; //DEBUG FUNC
            decode::decodedInstr d = decode::decode(instruction);
            execution::execute(vm, d);
        }
    }
}