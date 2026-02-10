#include "execution.h"
#include "vm.h"
#include "decode.h"

void execute(VM& vm, const decodedInstr& d) {
    switch (d.opcode) {
        case OP_ADD:
            add( vm.reg[d.ra], vm.reg[1], vm.reg[2]);
            break;
        case OP_SUB:
            sub( vm.reg[d.ra], vm.reg[1], vm.reg[2]);
            break;
        case OP_MUL:
            mul( vm.reg[d.ra], vm.reg[1], vm.reg[2]);
            break;
        case OP_DIV:
            div( vm.reg[d.ra], vm.reg[1], vm.reg[2]);
            break;
        case OP_RET:
            ret( vm.reg[d.ra]);
            vm.running = false;
            break;
        default:
            break;
    }
}