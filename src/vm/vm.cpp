#include "vm.h"
#include <format>
#include "bytecode.h"
#include "decode.h"
#include "execution.h"
#include "heap.h"
constexpr size_t HEAPSIZE = (512 * 1024 * 1024);

namespace sprout::vm {
    void init(VM& vm, const char* path) {
        vm.bytecode = bytecode::loadBytecode(path);
        vm.header = bytecode::loadHeader(vm.bytecode);
        bytecode::loadFunctionTable(vm.header, vm);
        vm.ip = vm.header.codeOffset;
        vm.heapAUsed = true;
        vm.heapA = new heap::HEAP();
        vm.heapB = new heap::HEAP();
        vm.heapA->max = HEAPSIZE;
        vm.heapB->max = HEAPSIZE;
        vm.heapA->totalAllocated = 0;
        vm.heapB->totalAllocated = 0;
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
        delete vm.heapA;
        delete vm.heapB;
    }
}