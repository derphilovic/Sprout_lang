#include <cstdint>
#include <iostream>
#include <cstring>
#include "vm/vm.h"
#include "vm/decode.h"
#include "vm/execution.h"

VM vm;
uint8_t* bytecode = 0;
int main() {
    init(vm, bytecode);
    vm.reg[0] = encodeToBytes(127.7);
    vm.reg[1] = encodeToBytes(0.3);
    run(vm);

    return 0;
}