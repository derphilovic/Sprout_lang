#include <cstdint>
#include <iostream>
#include <cstring>

uint64_t regs[256];

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

int main() {
    regs[0] = encodeToBytes(127.7);
    regs[1] = encodeToBytes(0.3);
    add(regs[2], regs[0], regs[1]);
    std::cout << decodeToDouble(regs[2]) << std::endl;

    return 0;

}