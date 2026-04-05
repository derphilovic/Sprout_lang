#pragma once
#include <cstdint>

enum Opcode : uint8_t {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_END,
    OP_LOAD64,
    OP_CMP,
    OP_DEBUG_RETURN,
    OP_JE, //8
    OP_JNE,
    OP_JL, //10
    OP_JG,
    OP_CALL, //12
    OP_RET,
    OP_PUSH, //14
    OP_POP,
    OP_INSERT_INTO_STACK,  //16
    OP_READ_FROM_STACK,
    OP_MOV, //18
    OP_ARR_INIT,
    OP_ARR_INSERT,
    OP_ARR_READ,

};
