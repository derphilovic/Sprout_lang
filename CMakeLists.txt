cmake_minimum_required(VERSION 4.0)
project(Sprout_lang)

set(CMAKE_CXX_STANDARD 20)

add_library(sprout_shared INTERFACE)
target_include_directories(sprout_shared INTERFACE src/shared)

add_executable(Sprout_lang
        src/main.cpp
        src/vm/vm.cpp
        src/vm/decode.cpp
        src/vm/bytecode.cpp
        src/vm/execution.cpp
        src/vm/heap.cpp
)

target_compile_options(Sprout_lang PRIVATE -O2)
target_link_libraries(Sprout_lang PRIVATE sprout_shared)

add_executable(sprout_compiler
        src/compiler/lexer.cpp
        src/compiler/compiler.cpp
        src/compiler/compiler.h
)

target_link_libraries(sprout_compiler PRIVATE sprout_shared)