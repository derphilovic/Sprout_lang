#include "vm/vm.h"
#include <iostream>

sprout::vm::VM vm;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: Sprout_lang <file.spbt>" << std::endl;
        return 1;
    }
    sprout::vm::init(vm, argv[1]);
    sprout::vm::run(vm);

    return 0;
}