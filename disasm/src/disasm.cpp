#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

#include "utils.h"
#include "disasm.h"

static void  turn_into_asm(disasm_config* disasm, FILE* file);
static char* scan_buffer(char* buffer, disasm_config* disasm);
static void  check_command(disasm_config* disasm);

void disasm_text(disasm_config* disasm, const char* byte_file, const char* asm_file)
{
    assert(disasm);
    assert(byte_file);

    disasm->buffer = get_file_content(byte_file);

    int ncommands = calc_nlines(disasm->buffer);

    FILE* file = fopen(asm_file, "w");

    char* buffer_ptr = disasm->buffer;
    for (int i = 0; i < ncommands; i++)
    {
        buffer_ptr = scan_buffer(buffer_ptr, disasm);
        check_command(disasm);
        turn_into_asm(disasm, file);
    }

    disasm->buffer = nullptr;
    free(disasm->buffer);

    fclose(file);
}

static char* scan_buffer(char* buffer, disasm_config* disasm)
{
    assert(buffer);
    assert(disasm);

    int shift = 0;
    while (buffer[shift++] != '\n');

    buffer[shift - 1] = '\0';

    if (sscanf(buffer, "%d %d %d", (int*) &disasm->masked_byte, &disasm->reg_num, &disasm->value) != 3)
    {
        sscanf(buffer, "%d %d", (int*) &disasm->masked_byte, &disasm->value);
    }

    return buffer + shift;
}

static void check_command(disasm_config* disasm)
{
    assert(disasm);

    int command_code = (int) disasm->masked_byte & 0b00001111;

    disasm->command = commands_to_string[command_code];

    unsigned char immed_reg_byte = disasm->masked_byte >> 4;

    if (immed_reg_byte == ARG_REG)
    {
        disasm->reg = registers_to_string[disasm->value];
    }

    if (immed_reg_byte == (ARG_REG | ARG_IMMED))
    {
        disasm->reg = registers_to_string[disasm->reg_num];
    }
}

static void turn_into_asm(disasm_config* disasm, FILE* file)
{
    assert(disasm);
    assert(file);

    unsigned char immed_reg_byte = disasm->masked_byte >> 4;

    if (immed_reg_byte == NO_ARG)
    {
        fprintf(file, "%s\n", disasm->command);
    }
    else if (immed_reg_byte == ARG_IMMED)
    {
        fprintf(file, "%s %d\n", disasm->command, disasm->value);
    }
    else if (immed_reg_byte == ARG_REG)
    {
        fprintf(file, "%s %s\n", disasm->command, disasm->reg);
    }
    else if (immed_reg_byte == (ARG_REG | ARG_IMMED))
    {
        fprintf(file, "%s %s + %d\n", disasm->command, disasm->reg, disasm->value);
    }
}
