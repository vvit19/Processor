#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

#include "utils.h"
#include "disasm.h"

#define RETURN(exit_code)     \
    if (exit_code == EXIT)    \
    {                         \
        free(disasm->labels); \
        free(buffer);         \
        return EXIT;          \
    }

#define JUMP_COMMAND(command) \
    (command == JMP) ||       \
    (command == JA ) ||       \
    (command == JAE) ||       \
    (command == JB ) ||       \
    (command == JBE) ||       \
    (command == JE ) ||       \
    (command == JNE) ||       \
    (command == CALL)

static void get_command(disasm_config* disasm, const char* asm_file);
static void        get_command_args(disasm_config* disasm, const char* asm_file);
static void        check_if_label(disasm_config* disasm, const char* asm_file);

void disasm_text(disasm_config* disasm, const char* byte_file, const char* asm_file)
{
    assert(disasm);
    assert(byte_file);
    assert(asm_file);

    char* buffer = get_file_content(byte_file);

    FILE* file = fopen(byte_file, "r");
    int file_size = get_file_size(file);
    fclose(file);

    disasm->labels = (char**) calloc(file_size, sizeof(char*));

    disasm->code = buffer;
    while (disasm->code != &buffer[file_size])
    {
        check_if_label(disasm, asm_file);
        disasm->command = *disasm->code++;
        get_command(disasm, asm_file);
    }

    free(disasm->labels);
    free(buffer);
}

static void get_command(disasm_config* disasm, const char* asm_file)
{
    assert(disasm);
    assert(asm_file);

    FILE* file = fopen(asm_file, "a");
    assert(file);

    #define DEF_CMD(string_name, name, ...) \
        case name:                          \
            fprintf(file, string_name " "); \
            break;

    switch (disasm->command & COMMAND)
    {
        #include "../../codegen/commands.h"

        default:
            fprintf(stderr, "Unknown case file: %s, line: %d\n", __FILE__, __LINE__);
            break;
    }

    #undef DEF_CMD

    fclose(file);

    get_command_args(disasm, asm_file);

    file = fopen(asm_file, "a");
    fprintf(file, "\n");
    fclose(file);
}

static void get_command_args(disasm_config* disasm, const char* asm_file)
{
    assert(disasm);
    assert(asm_file);

    FILE* file = fopen(asm_file, "a");

    if (JUMP_COMMAND((disasm->command & COMMAND)))
    {
        memcpy(&disasm->value, (elem_t*) disasm->code, sizeof(elem_t)); disasm->code += sizeof(elem_t);
        char* label = disasm->code - (int) disasm->value;
        disasm->labels[disasm->labels_ip++] = label;

        fprintf(file, "Label_%d", disasm->labels_ip);
        fclose(file);
        return;
    }

    if ((disasm->command & ARG_IMMED) &&
        (disasm->command & ARG_REG))
    {
        disasm->reg_num = *disasm->code++;
        disasm->value   = *((elem_t*) disasm->code); disasm->code += sizeof(elem_t);

        if (disasm->command & ARG_MEM)
        {
            fprintf(file, "[%s + %lf]", registers_to_string[(int) disasm->reg_num], disasm->value);

            fclose(file);
            return;
        }

        fprintf(file, "%s + %lf", registers_to_string[(int) disasm->reg_num], disasm->value);

        fclose(file);
        return;
    }

    if (disasm->command & ARG_IMMED)
    {
        memcpy(&disasm->value, (elem_t*) disasm->code, sizeof(elem_t)); disasm->code += sizeof(elem_t);

        if (disasm->command & ARG_MEM)
        {
            fprintf(file, "[%lf]", disasm->value);

            fclose(file);
            return;
        }

        fprintf(file, "%lf", disasm->value);
    }

    if (disasm->command & ARG_REG)
    {
        disasm->reg_num = *disasm->code++;

        if (disasm->command & ARG_MEM)
        {
            fprintf(file, "[%s]", registers_to_string[(int) disasm->reg_num]);
        }

        fprintf(file, "%s", registers_to_string[(int) disasm->reg_num]);
    }

    fclose(file);
}

static void check_if_label(disasm_config* disasm, const char* asm_file)
{
    assert(disasm);
    assert(asm_file);

    for (int i = 0; i < disasm->labels_ip; i++)
    {
        if ((disasm->code + sizeof(elem_t)) == disasm->labels[i])
        {
            FILE* file = fopen(asm_file, "a");
            fprintf(file, "Label_%d\n", i + 1);
            fclose(file);

            return;
        }
    }
}
