#ifndef SPROUT_LANG_EXECUTION_H
#define SPROUT_LANG_EXECUTION_H
#include <cstdint>
#include <iostream>
#include <ostream>
#include "decode.h"
#include "vm.h"


namespace sprout::execution {
    void execute(vm::VM& vm, const decode::decodedInstr& d);

    enum Opcode : uint8_t {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_RET,
        OP_LOAD64,
        OP_CMP,
        OP_DEBUG_RETURN,
        OP_JE,
        OP_JNE,
        OP_JL,
        OP_JG,
    };

    inline void add(uint64_t& dst, uint64_t a, uint64_t b) {
        double da = decode::decodeToDouble(a);
        double db = decode::decodeToDouble(b);
        double res = da + db;
        dst = decode::encodeToBytes(res);
    }

    inline void sub(uint64_t& dst, uint64_t a, uint64_t b) {
        double da = decode::decodeToDouble(a);
        double db = decode::decodeToDouble(b);
        double res = da - db;
        dst = decode::encodeToBytes(res);
    }

    inline void mul(uint64_t& dst, uint64_t a, uint64_t b) {
        double da = decode::decodeToDouble(a);
        double db = decode::decodeToDouble(b);
        double res = da * db;
        dst = decode::encodeToBytes(res);
    }

    inline void div(uint64_t& dst, uint64_t a, uint64_t b) {
        double da = decode::decodeToDouble(a);
        double db = decode::decodeToDouble(b);
        double res = da / db;
        dst = decode::encodeToBytes(res);
    }

    inline void loadIntoReg(vm::VM& vm, uint8_t reg) {
        uint64_t val = fetch64(vm);
        vm.reg[reg] = val;
    }

    inline void ret(uint64_t ret) {
        double r = decode::decodeToDouble(ret);
        std::cout << r << std::endl;
    }

    inline void compare(vm::VM &vm, uint64_t a, uint64_t b) {
        double da = decode::decodeToDouble(a);
        double db = decode::decodeToDouble(b);

        if (da == db) vm.jmpFlag = 1;
        else if (da < db) vm.jmpFlag = -1;
        else if (da > db) vm.jmpFlag = 2;
        else vm.jmpFlag = 0;
    }

    inline void je(vm::VM& vm) {
        if (vm.jmpFlag == 1) vm.ip = vm::fetch(vm);
        else vm.ip += 4;
    }

    inline void jne(vm::VM& vm) {
        if (vm.jmpFlag != 1) vm.ip = vm::fetch(vm);
        else vm.ip += 4;
    }

    inline void jl(vm::VM& vm) {
        if (vm.jmpFlag  == -1) vm.ip = vm::fetch(vm);
        else vm.ip += 4;
    }

    inline void jg(vm::VM& vm) {
        if (vm.jmpFlag == 2)  vm.ip = vm::fetch(vm);
        else vm.ip += 4;
    }

}
class execution {
};


#endif //SPROUT_LANG_EXECUTION_H