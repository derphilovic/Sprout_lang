cmake_minimum_required(VERSION 4.0)
project(Sprout_lang)

set(CMAKE_CXX_STANDARD 14)

add_executable(Sprout_lang
        src/main.cpp
        src/vm/vm.cpp
        src/vm/vm.h
        src/vm/decode.cpp
        src/vm/decode.h
        src/vm/bytecode.cpp
        src/vm/bytecode.h
        src/vm/execution.cpp
        src/vm/execution.h
)
