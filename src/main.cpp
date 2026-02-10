#include <cstdint>
#include <iostream>
#include <cstring>
#include "vm/vm.h"
#include "vm/decode.h"
#include "vm/execution.h"

VM vm;

int main() {
    vm.reg[0] = encodeToBytes(127.7);
    vm.reg[1] = encodeToBytes(0.3);
    add(vm.reg[2], vm.reg[0], vm.reg[1]);
    std::cout << decodeToDouble(vm.reg[2]) << std::endl;

    return 0;

}