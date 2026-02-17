#include "execution.h"

#include <format>

#include "vm.h"
#include "decode.h"

namespace sprout::execution {
    void execute(vm::VM& vm, const decode::decodedInstr& d) {
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
            case OP_CMP:
                compare(vm, vm.reg[d.ra], vm.reg[d.rb]);
                break;
            case OP_JE:
                je(vm);
                break;
            case OP_JNE:
                jne(vm);
                break;
            case OP_JL:
                jl(vm);
                break;
            case OP_JG:
                jg(vm);
            case OP_RET:
                ret( vm.reg[d.ra]);
                vm.running = false;
                break;
            case OP_DEBUG_RETURN:
                ret(vm.reg[d.ra]);
                break;
            default:
                throw std::runtime_error(std::format("Invalid opcode: 0x{:02X}", d.opcode));
                break;
        }
    }
}