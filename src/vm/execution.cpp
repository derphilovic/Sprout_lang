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
                break;
            case OP_CALL:
                call(vm, d.ra, d.rb, d.rc);
                break;
            case OP_RET:
                ret(vm, d.ra, d.rb, d.rc);
                break;
            case OP_PUSH:
                push(vm, vm.reg[d.ra]);
                break;
            case OP_POP:
                pop(vm, vm.reg[d.ra]);
                break;
            case OP_INSERT_INTO_STACK:
                insertIntoStack(vm, d.ra, d.rb, vm.reg[d.rc]);
                break;
            case OP_READ_FROM_STACK:
                readFromStack(vm, d.ra, d.rb, vm.reg[d.rc]);
                break;
            case OP_END:
                end( vm.reg[d.ra]);
                vm.running = false;
                break;
            case OP_DEBUG_RETURN:
                end(vm.reg[d.ra]);
                break;
            default:
                throw std::runtime_error(std::format("Invalid opcode: 0x{:02X}", d.opcode));
                break;
        }
    }
}