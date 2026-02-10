#ifndef SPROUT_LANG_EXECUTION_H
#define SPROUT_LANG_EXECUTION_H
#include <cstdint>
#include "decode.h"

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

class execution {
};


#endif //SPROUT_LANG_EXECUTION_H