#ifndef DISASM_H
#define DISASM_H

const char* const commands_to_string[] = {"hlt", "out", "in", "push", "pop", "add", "sub", "mult", "div"};

const char* const registers_to_string[] = {"rax", "rbx", "rcx", "rdx"};

enum byte_masks
{
    NO_ARG    = 0,
    ARG_IMMED = 1,
    ARG_REG   = 2,
};

struct disasm_config
{
    char* buffer;
    const char* command;
    int value;
    int reg_num;
    const char* reg;
    unsigned char masked_byte;
};

void disasm_text(disasm_config* disasm, const char* byte_file, const char* asm_file);

#endif
