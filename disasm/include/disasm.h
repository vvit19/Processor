#ifndef DISASM_H
#define DISASM_H

typedef double elem_t;

// #define DEF_CMD(string_name, ...) string_name,
// const char* const commands_to_string[]  =
// {
//     #include "commands.h"
// };
// #undef DEF_CMD

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

const char* const registers_to_string[] = {"rax", "rbx", "rcx", "rdx"};

enum byte_masks
{
    ARG_IMMED = 0b00100000,
    ARG_REG   = 0b01000000,
    ARG_MEM   = 0b10000000,
    COMMAND   = 0b00011111,
};

struct disasm_config
{
    char*  code;
    char** labels;
    int    labels_ip;
    char   command;
    elem_t value;
    char   reg_num;
};

void disasm_text(disasm_config* disasm, const char* byte_file, const char* asm_file);

#endif
