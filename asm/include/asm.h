#ifndef ASM_H
#define ASM_H

#define DEF_CMD(string_name, ...) string_name,
const char* const commands_to_string[]  =
{
    #include "commands.h"
};
#undef DEF_CMD

const char* const registers_to_string[] = {"rax", "rbx", "rcx", "rdx"};

enum byte_masks
{
    ARG_IMMED = 0b00100000,
    ARG_REG   = 0b01000000,
    ARG_MEM   = 0b10000000,
};

struct named_label
{
    char  name[20];
    char* label_ptr;
};

struct asm_config
{
    char*        code;
    char*        buffer;
    char*        label;
    char**       labels;
    char         command[20];
    double       value;
    char         reg[4];
    char         reg_code;
    char         masked_byte;
    named_label* named_labels_array;
    int          named_labels_ip;
    char*        position;
};

struct brackets
{
    char* bracket_1;
    char* bracket_2;
};

void run_asm(asm_config* asm_parameters, const char* asm_file, const char* byte_file);

#endif
