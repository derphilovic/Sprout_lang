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
        OP_END,
        OP_LOAD64,
        OP_CMP,
        OP_DEBUG_RETURN,
        OP_JE,
        OP_JNE,
        OP_JL,
        OP_JG,
        OP_CALL,
        OP_RET,
        OP_PUSH,
        OP_POP,
        OP_INSERT_INTO_STACK,
        OP_READ_FROM_STACK
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

    inline void end(uint64_t ret) {
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

    inline void call(vm::VM& vm, uint8_t a, uint8_t b, uint8_t c) {
        uint32_t index  = (static_cast<uint32_t>(a) << 16) | (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(c); // Get index of Function
        vm::functionInfo f = vm.functionTable.at(index); // Getting function Metadata
        decode::push(vm, vm.fp); // Push FP
        decode::push(vm, vm.ip); // Push IP
        vm.fp = vm.sp; // Set new FP
        vm.sp += f.frameSize; // Reserve space for local variables
        vm.ip = f.entryIP; // Setting IP to function entry
    }

    inline void ret(vm::VM& vm, uint8_t a, uint8_t b, uint8_t c) {
        uint32_t index  = (static_cast<uint32_t>(a) << 16) | (static_cast<uint32_t>(b) << 8) | static_cast<uint32_t>(c); // Get index of Function
        vm::functionInfo f = vm.functionTable.at(index); // Getting function Metadata

        vm.sp -= f.frameSize; // Deallocating reserved space for local vars or arguments
        vm.ip = decode::pop(vm); // Restore IP
        vm.fp = decode::pop(vm); // Restore FP
    }

    inline void push(vm::VM& vm, __uint64_t target) {
        decode::push(vm, target);
    }

    inline void pop(vm::VM& vm, uint64_t& dst) {
        dst = decode::pop(vm);
    }

    inline void insertIntoStack(vm::VM& vm, uint8_t a, uint8_t b, uint64_t target) {
        uint16_t point = (static_cast<uint16_t>(a) << 8) | static_cast<uint16_t>(b);
        vm.stack[point] = target;
    }

    inline void readFromStack(vm::VM& vm, uint8_t a, uint8_t b, uint64_t& target) {
        uint16_t point = (static_cast<uint16_t>(a) << 8) | static_cast<uint16_t>(b);
        target = vm.stack[point];
    }

}

#endif //SPROUT_LANG_EXECUTION_H