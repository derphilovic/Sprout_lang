#ifndef SPROUT_LANG_EXECUTION_H
#define SPROUT_LANG_EXECUTION_H
#include <cstdint>
#include <iostream>
#include <ostream>
#include "decode.h"
#include "vm.h"

void execute(VM& vm, const decodedInstr& d);


enum Opcode : uint8_t {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_RET,
    OP_LOAD64,
};

inline void add(uint64_t& dst, uint64_t a, uint64_t b) {
    double da = decodeToDouble(a);
    double db = decodeToDouble(b);
    double res = da + db;
    dst = encodeToBytes(res);
}

inline void sub(uint64_t& dst, uint64_t a, uint64_t b) {
    double da = decodeToDouble(a);
    double db = decodeToDouble(b);
    double res = da - db;
    dst = encodeToBytes(res);
}

inline void mul(uint64_t& dst, uint64_t a, uint64_t b) {
    double da = decodeToDouble(a);
    double db = decodeToDouble(b);
    double res = da * db;
    dst = encodeToBytes(res);
}

inline void div(uint64_t& dst, uint64_t a, uint64_t b) {
    double da = decodeToDouble(a);
    double db = decodeToDouble(b);
    double res = da / db;
    dst = encodeToBytes(res);
}

inline void loadIntoReg(VM& vm, uint8_t reg) {
    uint64_t val = (static_cast<uint64_t>(fetch(vm)) << 32) | fetch(vm);
    vm.reg[reg] = val;
}

inline void ret(uint64_t ret) {
    double r = decodeToDouble(ret);
    std::cout << r << std::endl;
}

class execution {
};


#endif //SPROUT_LANG_EXECUTION_H