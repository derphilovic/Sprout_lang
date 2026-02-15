#include <cstdint>
#include <iostream>
#include <cstring>
#include "vm/vm.h"
#include "vm/decode.h"
#include "vm/execution.h"

sprout::vm::VM vm;

int main() {
    sprout::vm::init(vm, "test.spbt");
    sprout::vm::run(vm);

    return 0;
}