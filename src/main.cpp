#include <cstdint>
#include <iostream>
#include <cstring>
#include "vm/vm.h"
#include "vm/decode.h"
#include "vm/execution.h"

VM vm;

int main() {
    init(vm, "test.spbt");
    vm.reg[0] = encodeToBytes(127.7);
    vm.reg[1] = encodeToBytes(0.3);
    run(vm);

    return 0;
}