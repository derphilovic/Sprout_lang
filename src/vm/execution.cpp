#include "execution.h"

#include <format>

#include "vm.h"
#include "decode.h"

void execute(VM& vm, const decodedInstr& d) {
    switch (d.opcode) {
        case OP_ADD:
            add( vm.reg[d.ra], vm.reg[d.rb], vm.reg[d.rc]);
            break;
        case OP_SUB:
            sub( vm.reg[d.ra], vm.reg[d.rb], vm.reg[d.rc]);
            break;
        case OP_MUL:
            mul( vm.reg[d.ra], vm.reg[d.rb], vm.reg[d.rc]);
            break;
        case OP_DIV:
            div( vm.reg[d.ra], vm.reg[d.rb], vm.reg[d.rc]);
            break;
        case OP_LOAD64:
            loadIntoReg(vm, d.ra);
            break;
        case OP_RET:
            ret( vm.reg[d.ra]);
            vm.running = false;
            break;
        default:
            throw std::runtime_error(std::format("Invalid opcode: 0x{:02X}", d.opcode));
            break;
    }
}