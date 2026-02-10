#ifndef SPROUT_LANG_DECODE_H
#define SPROUT_LANG_DECODE_H
#include <cstdint>
#include <cstring>

inline double decodeToDouble(uint64_t reg) {
    double d;
    std::memcpy(&d, &reg, sizeof(double));
    return d;
}

inline uint64_t encodeToBytes(double d) {
    int64_t i;
    std::memcpy(&i, &d, sizeof(double));
    return i;
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

#endif //SPROUT_LANG_DECODE_H