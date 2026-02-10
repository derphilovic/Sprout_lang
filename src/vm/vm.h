#ifndef SPROUT_LANG_VM_H
#define SPROUT_LANG_VM_H
#include <cstdint>

struct VM {
    uint64_t reg[256];
    uint8_t* bytecode;
    std::size_t bytecode_size;
    std::size_t ip;

    bool running;
};

void init(VM& vm, uint8_t* bytecode);
void run(VM& vm);

class vm {
};

#endif //SPROUT_LANG_VM_H