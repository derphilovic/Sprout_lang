#pragma once
#include <cstdint>
#include <cstring>

constexpr uint64_t NAN_BASE = 0x7FF0000000000000ULL;
constexpr uint64_t NAN_PAYLOAD = 0x0000FFFFFFFFFFFFULL;

enum tag : uint64_t {
    TAG_DOUBLE,
    TAG_INT48,
    TAG_POINTER,
    TAG_BOOL,
    TAG_CHAR6,
    TAG_NULL
};

struct char6 {
    char a, b, c, d, e, f;
};

inline bool isDouble(uint64_t v) {
    // If all exponent bits (62-52) are set, it's a NaN → not a plain double
    return (v & NAN_BASE) != NAN_BASE;
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

inline bool isBool(uint64_t v) {
    return !isDouble(v) && getTag(v) == TAG_BOOL;
}

inline bool isChar6(uint64_t v) {
    return !isDouble(v) && getTag(v) == TAG_CHAR6;
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

inline uint64_t encodeInt(uint64_t reg) {
    return NAN_BASE | (uint64_t(TAG_INT48) << 48) | ( reg & NAN_PAYLOAD);
}

inline int64_t decodeInt(uint64_t num) {
    int64_t enc = num & NAN_PAYLOAD;
    if (enc & (1LL << 47)) enc |= 0xFFFF000000000000ULL;
    return enc;
}

inline uint64_t encodePointer(uint64_t v) {
    return NAN_BASE | (uint64_t(TAG_POINTER) << 48) | (v & NAN_PAYLOAD);
}

inline void* decodePointer(uint64_t r) {
    return reinterpret_cast<void*>(r & NAN_PAYLOAD);
}

inline uint64_t encodeBool(bool b) {
    return NAN_BASE | (uint64_t(TAG_BOOL) << 48) | (b ? 1 : 0);
}

inline bool decodeBool(uint64_t r) {
    return r & 1;
}

inline uint64_t encodeChar6(char6 c) {
    uint64_t value = 0;
    std::memcpy(&value, &c, 6);
    return NAN_BASE | (uint64_t(TAG_CHAR6) << 48) | (value & NAN_PAYLOAD) ;
}

inline char6 decodeChar6(uint64_t r) {
    uint64_t v = r & NAN_PAYLOAD;
    char6 c;
    std::memcpy(&c, &v, 6);
    return c;
}