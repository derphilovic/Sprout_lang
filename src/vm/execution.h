#ifndef SPROUT_LANG_EXECUTION_H
#define SPROUT_LANG_EXECUTION_H
#include <cstdint>
#include <iostream>
#include <ostream>
#include "decode.h"
#include "vm.h"
#include <values.h>


namespace sprout::execution {
    void execute(vm::VM& vm, const decode::decodedInstr& d);


    inline void add(uint64_t& dst, uint64_t a, uint64_t b) {
        if (isDouble(a) && isDouble(b)) {
            double da = decodeDouble(a);
            double db = decodeDouble(b);
            double res = da + db;
            dst = encodeDouble(res);
        }else if (isInt(a) && isInt(b)) {
            int64_t da = decodeINT(a);
            int64_t db = decodeINT(b);
            int64_t res = da + db;
            dst = encodeINT(res);
        }else {
            throw std::runtime_error("Invalid Operand types!");
        }
    }

    inline void sub(uint64_t& dst, uint64_t a, uint64_t b) {
        if (isDouble(a) && isDouble(b)) {
            double da = decodeDouble(a);
            double db = decodeDouble(b);
            double res = da - db;
            dst = encodeDouble(res);
        }else if (isInt(a) && isInt(b)) {
            int64_t da = decodeINT(a);
            int64_t db = decodeINT(b);
            int64_t res = da - db;
            dst = encodeINT(res);
        }else {
            throw std::runtime_error("Invalid Operand types!");
        }
    }

    inline void mul(uint64_t& dst, uint64_t a, uint64_t b) {
        if (isDouble(a) && isDouble(b)) {
            double da = decodeDouble(a);
            double db = decodeDouble(b);
            double res = da * db;
            dst = encodeDouble(res);
        }else if (isInt(a) && isInt(b)) {
            int64_t da = decodeINT(a);
            int64_t db = decodeINT(b);
            int64_t res = da * db;
            dst = encodeINT(res);
        }else {
            throw std::runtime_error("Invalid Operand types!");
        }
    }

    inline void div(uint64_t& dst, uint64_t a, uint64_t b) {
        if (isDouble(a) && isDouble(b)) {
            double da = decodeDouble(a);
            double db = decodeDouble(b);
            double res = da / db;
            dst = encodeDouble(res);
        }else if (isInt(a) && isInt(b)) {
            int64_t da = decodeINT(a);
            int64_t db = decodeINT(b);
            int64_t res = da / db;
            dst = encodeINT(res);
        }else {
            throw std::runtime_error("Invalid Operand types!");
        }
    }

    inline void loadIntoReg(vm::VM& vm, uint8_t reg) {
        uint64_t val = fetch64(vm);
        vm.reg[reg] = val;
    }

    inline void end(uint64_t ret) {
        if (isInt(ret)) {
            int64_t r = decodeINT(ret);
            std::cout << r << std::endl;
        } else if (isDouble(ret)) {
            double r = decodeDouble(ret);
            std::cout << r << std::endl;
        }
    }

    inline void compare(vm::VM &vm, uint64_t a, uint64_t b) {
        if (isDouble(a) && isDouble(b)) {
            double da = decodeDouble(a);
            double db = decodeDouble(b);

            if (da == db) vm.jmpFlag = 1;
            else if (da < db) vm.jmpFlag = -1;
            else if (da > db) vm.jmpFlag = 2;
            else vm.jmpFlag = 0;
        } else if (isInt(a) && isInt(b)) {
            int64_t da = decodeINT(a);
            int64_t db = decodeINT(b);

            if (da == db) vm.jmpFlag = 1;
            else if (da < db) vm.jmpFlag = -1;
            else if (da > db) vm.jmpFlag = 2;
            else vm.jmpFlag = 0;
        } else if (isBool(a) && isBool(b)) {
            bool da = decodeBool(a);
            bool db = decodeBool(b);

            if (da == db) vm.jmpFlag = 1;
            else vm.jmpFlag = 0;
            } else {
            throw std::runtime_error("Invalid Operand types");
        }
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
        vm::functionInfo f = vm.functionTable[index]; // Getting function Metadata
        decode::push(vm, vm.fp); // Push FP
        decode::push(vm, vm.ip); // Push IP
        vm.fp = vm.sp; // Set new FP
        vm.sp += f.frameSize; // Reserve space for local variables
        vm.ip = f.entryIP; // Setting IP to function entry
    }

    inline void ret(vm::VM& vm, uint8_t a, uint8_t b, uint8_t c) {
        vm.sp = vm.fp; // Deallocating reserved space for local vars or arguments
        vm.ip = decode::pop(vm); // Restore IP
        vm.fp = decode::pop(vm); // Restore FP
    }

    inline void push(vm::VM& vm, uint64_t target) {
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

    inline void mov(vm::VM& vm, uint8_t dst, uint8_t src, uint8_t flag) {
        switch (flag) {
            case 1:
                vm.reg[dst] = *reinterpret_cast<uint64_t*>(vm.reg[src]); break;
            case 2:
                *reinterpret_cast<uint64_t*>(vm.reg[dst]) = vm.reg[src]; break;
            case 3:
                *reinterpret_cast<uint64_t*>(vm.reg[dst]) = *reinterpret_cast<uint64_t*>(vm.reg[src]); break;
            default:vm.reg[dst] = vm.reg[src]; break;
        }
    }

}

#endif //SPROUT_LANG_EXECUTION_H