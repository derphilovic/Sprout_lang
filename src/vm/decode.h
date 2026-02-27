#ifndef SPROUT_LANG_DECODE_H
#define SPROUT_LANG_DECODE_H
#include <cstdint>
#include <cstring>
#include "vm.h"

namespace sprout::decode {
    constexpr uint64_t NAN_BASE = 0x7FF0000000000000ULL;

    struct decodedInstr {
        uint8_t opcode;
        uint8_t ra;
        uint8_t rb;
        uint8_t rc;
    };

    enum tag : uint64_t {
        TAG_DOUBLE,
        TAG_INT48,
        TAG_POINTER,
        TAG_BOOL,
        TAG_CHAR6,
        TAG_NULL
    };

    inline bool isDouble(uint64_t v) {
        // If all exponent bits (62-52) are set, it's a NaN → not a plain double
        return (v & 0x7FF0000000000000ULL) != 0x7FF0000000000000ULL;
    }

    inline uint8_t getTag(uint64_t v) {
        return (v >> 48) & 0xF;
    }

    inline bool isInt(uint64_t v) {
        return !isDouble(v) && getTag(v) == TAG_INT48;
    }

    inline bool isPointer(uint64_t v) {
        return !isDouble(v) && getTag(v) == TAG_POINTER;
    }

    inline double decodeDouble(uint64_t reg) {
        double d;
        std::memcpy(&d, &reg, sizeof(double));
        return d;
    }

    inline uint64_t encodeDouble(double d) {
        int64_t i;
        std::memcpy(&i, &d, sizeof(double));
        return i;
    }

    inline uint64_t encodeINT(uint64_t reg) {
        return NAN_BASE | (uint64_t(TAG_INT48) << 48) | ( reg & 0x0000FFFFFFFFFFFFULL);
    }

    inline int64_t decodeINT(uint64_t num) {
        int64_t enc = num & 0x0000FFFFFFFFFFFFULL;
        if (enc & (1LL << 47)) enc |= 0xFFFF000000000000ULL;
        return enc;
    }

    inline uint64_t encodePointer(uint64_t v) {
        return NAN_BASE | (uint64_t(TAG_POINTER) << 48) | (v & 0x0000FFFFFFFFFFFFULL);
    }

    inline void* decodePointer(uint64_t r) {
        return reinterpret_cast<void*>(r & 0x0000FFFFFFFFFFFFULL);
    }

    inline uint8_t op(uint32_t instr) {
        return instr >> 24;
    }

    inline uint8_t ra(uint32_t instr) {
        return (instr >> 16) & 0xFF;
    }

    inline uint8_t rb(uint32_t instr) {
        return (instr >> 8) & 0xFF;
    }

    inline uint8_t rc(uint32_t instr) {
        return instr & 0xFF;
    }

    inline decodedInstr decode(uint32_t instr) {
        decodedInstr decInstr{};
        decInstr.opcode = op(instr);
        decInstr.ra = ra(instr);
        decInstr.rb = rb(instr);
        decInstr.rc = rc(instr);
        return decInstr;
    }

    inline void push(vm::VM& vm, uint64_t a) {
        if (vm.sp >= 4096) {
            throw std::runtime_error("!!!STACK OVERFLOW!!!");;
        }
        vm.stack[vm.sp++] = a;
    }

    inline uint64_t pop(vm::VM& vm) {
        return vm.stack[--vm.sp];
    }

}
#endif //SPROUT_LANG_DECODE_H