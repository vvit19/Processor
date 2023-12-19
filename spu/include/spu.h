#ifndef PROC_H
#define PROC_H

#include "stack.h"

const int RAM_SIZE = 121;

enum byte_masks
{
    ARG_IMMED = 0b00100000,
    ARG_REG   = 0b01000000,
    ARG_MEM   = 0b10000000,
    COMMAND   = 0b00011111,
};

enum commands
{
    HLT ,
    OUT ,
    OUTC,
    IN  ,
    PUSH,
    POP ,
    ADD ,
    SUB ,
    MULT,
    DIV ,
    SIN ,
    COS ,
    SQRT,
    JMP ,
    JA  ,
    JAE ,
    JB  ,
    JBE ,
    JE  ,
    JNE ,
    CALL,
    RET ,
};

struct regs
{
    elem_t rax;
    elem_t rbx;
    elem_t rcx;
    elem_t rdx;
};

struct spu
{
    elem_t* ram;
    char*   code;
    char    command;
    elem_t  value;
    char    reg_num;
    regs    reg;
    stack*  stk;
};

enum return_code
{
    EXIT     = 0,
    CONTINUE = 1,
};

return_code spu_start(spu* proc, const char* byte_file);

#endif
