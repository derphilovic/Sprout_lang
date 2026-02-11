#include <cstdint>
#include <iostream>
#include <cstring>
#include "vm/vm.h"
#include "vm/decode.h"
#include "vm/execution.h"

VM vm;
uint8_t code[8] = {
    0x00, 0x02, 0x01, 0x00,
    0x04, 0x02, 0x10, 0x00
};
uint8_t* bytecode = code;
int main() {
    init(vm, bytecode);
    vm.reg[0] = encodeToBytes(127.7);
    vm.reg[1] = encodeToBytes(0.3);
    run(vm);

    return 0;
}