#include <cstdint>
#include <iostream>
#include <cstring>
#include "vm/vm.h"
#include "vm/decode.h"
#include "vm/execution.h"

VM vm;

int main() {
    init(vm, "test.spbt");
    run(vm);

    return 0;
}