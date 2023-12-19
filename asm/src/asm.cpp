#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>

#include "utils.h"
#include "asm.h"

#define CHECK_JUMP_COMMANDS                                 \
    (strcmp(asm_parameters->command, "jump") == 0) ||       \
    (strcmp(asm_parameters->command, "ja"  ) == 0) ||       \
    (strcmp(asm_parameters->command, "jae" ) == 0) ||       \
    (strcmp(asm_parameters->command, "jb"  ) == 0) ||       \
    (strcmp(asm_parameters->command, "jbe" ) == 0) ||       \
    (strcmp(asm_parameters->command, "je"  ) == 0) ||       \
    (strcmp(asm_parameters->command, "jne" ) == 0) ||       \
    (strcmp(asm_parameters->command, "call") == 0)


static void  turn_into_bytecode(asm_config* asm_parameters);
static char* scan_buffer(asm_config* asm_parameters);
static void  check_command_args(asm_config* asm_parameters);
static void  check_memory_args(asm_config* asm_parametes, brackets* brackets_info);
static void  fill_brackets_back(brackets* brackets);
static void  check_command(asm_config* asm_parameters);
static void  check_register(asm_config* asm_parameters);
static void  roll_parameters_back(asm_config* asm_parameters);
static char* find_label(asm_config* asm_parameters);
static void  asm_functions_call(asm_config* asm_parameters, int ncommands);
static char* find_named_label(asm_config* asm_parameters);

const double VALUE_GARBAGE = 0xDEADFA11;
const double NAMED_LABEL   = 0xDEDADEDA;

void run_asm(asm_config* asm_parameters, const char* asm_file, const char* byte_file)
{
    assert(asm_parameters);
    assert(asm_file);
    assert(byte_file);

    asm_parameters->buffer = get_file_content(asm_file);
    char* buffer_ptr = asm_parameters->buffer;

    int ncommands = calc_nlines(asm_parameters->buffer);

    asm_parameters->code = (char*) calloc(ncommands * (2 * sizeof(char) + sizeof(double)) * 2, sizeof(char));
    char* code_ptr = asm_parameters->code;

    asm_parameters->labels             = (char**)       calloc(ncommands,       sizeof(char*));
    asm_parameters->named_labels_array = (named_label*) calloc(ncommands, sizeof(named_label));

    asm_functions_call(asm_parameters, ncommands);

    asm_parameters->code = code_ptr;
    asm_parameters->buffer = buffer_ptr;

    asm_functions_call(asm_parameters, ncommands);

    FILE* file = fopen(byte_file, "wb");
    fwrite(code_ptr, asm_parameters->code - code_ptr + 1, sizeof(char), file);
    fclose(file);

    free(buffer_ptr);
    free(asm_parameters->named_labels_array);
    free(code_ptr);
    free(asm_parameters->labels);
}

static void asm_functions_call(asm_config* asm_parameters, int ncommands)
{
    assert(asm_parameters);
    assert(asm_parameters->buffer);

    for (int line = 0; line < ncommands; line++)
    {
        asm_parameters->buffer = remove_empty_lines(asm_parameters->buffer, &line, ncommands);
        if (line >= ncommands) return;

        asm_parameters->buffer = scan_buffer(asm_parameters);

        check_command(asm_parameters);
        check_command_args(asm_parameters);
        check_register(asm_parameters);

        turn_into_bytecode(asm_parameters);

        roll_parameters_back(asm_parameters);
    }
}

static char* scan_buffer(asm_config* asm_parameters)
{
    assert(asm_parameters->buffer);
    assert(asm_parameters);

    int i = skip_spaces (asm_parameters->buffer);

    asm_parameters->buffer += i;

    int shift = calc_symbols_in_line(&asm_parameters->buffer);

    sscanf(asm_parameters->buffer, "%s", asm_parameters->command);
    int command_length = strlen(asm_parameters->command);

    brackets brackets_info = {};
    check_memory_args(asm_parameters, &brackets_info);

    char* position = asm_parameters->buffer + command_length + 1;

    if (asm_parameters->command[command_length - 1] == ':')
    {
        asm_parameters->command[command_length - 1] = '\0';
        strcpy(asm_parameters->named_labels_array[asm_parameters->named_labels_ip].name, asm_parameters->command);
        asm_parameters->named_labels_array[asm_parameters->named_labels_ip].label_ptr = asm_parameters->code;
        asm_parameters->named_labels_ip++;
        asm_parameters->command[0] = '\0';
    }

    if (CHECK_JUMP_COMMANDS)
    {
        if (sscanf(position + 1, "%lf", &asm_parameters->value) != 1)
        {
            asm_parameters->value = NAMED_LABEL;
            asm_parameters->position = position;
        }

        asm_parameters->label = asm_parameters->code;
    }
    else
    {
        find_label(asm_parameters);
    }

    if (strcmp(asm_parameters->command, "push") == 0)
    {
        if (sscanf(position, "%s + %lf", asm_parameters->reg, &asm_parameters->value) != 2)
        {
            if (sscanf(position, "%lf", &asm_parameters->value) == 1)
            {
                asm_parameters->reg[0] = '\0';
            }
            else
            {
                sscanf(position, "%s", asm_parameters->reg);
            }
        }
    }

    if (strcmp(asm_parameters->command, "pop") == 0)
    {
        sscanf(position, "%s", asm_parameters->reg);
    }

    fill_brackets_back(&brackets_info);

    return asm_parameters->buffer + shift;
}

static char* find_named_label(asm_config* asm_parameters)
{
    assert(asm_parameters);

    char label_name[20] = "";

    sscanf(asm_parameters->position, "%s", label_name);

    for (int i = 0; i < asm_parameters->named_labels_ip; i++)
    {
        if (strcmp(asm_parameters->named_labels_array[i].name, label_name) == 0)
        {
            return asm_parameters->named_labels_array[i].label_ptr;
        }
    }

    return nullptr;
}

static void check_memory_args(asm_config* asm_parameters, brackets* brackets_info)
{
    assert(asm_parameters);

    for (int i = 0; (asm_parameters->buffer[i] != '\n') && (asm_parameters->buffer[i] != '\0'); i++)
    {
        if ((asm_parameters->buffer[i] == ']') && (brackets_info->bracket_1 != nullptr))
        {
            asm_parameters->buffer[i] = ' ';

            brackets_info->bracket_2 = &asm_parameters->buffer[i];

            asm_parameters->masked_byte |= ARG_MEM;
            return;
        }

        if (asm_parameters->buffer[i] == '[')
        {
            asm_parameters->buffer[i] = ' ';

            brackets_info->bracket_1 = &asm_parameters->buffer[i];
        }
    }
}

static void fill_brackets_back(brackets* brackets)
{
    assert(brackets);

    if (brackets->bracket_1 != nullptr) *(brackets->bracket_1) = '[';
    if (brackets->bracket_2 != nullptr) *(brackets->bracket_2) = ']';
}

static void check_command(asm_config* asm_parameters)
{
    assert(asm_parameters);

    for (size_t i = 0; i < sizeof(commands_to_string) / sizeof(char*); i++)
    {
        if (strcmp(asm_parameters->command, commands_to_string[i]) == 0)
        {
            asm_parameters->masked_byte += i;
            break;
        }
    }
}

static void check_register(asm_config* asm_parameters)
{
    assert(asm_parameters);

    if (asm_parameters->reg[0] == '\0') return;

    for (size_t i = 0; i < sizeof(registers_to_string) / sizeof(char*); i++)
    {
        if (strcmp(asm_parameters->reg, registers_to_string[i]) == 0)
        {
            asm_parameters->reg_code = (char) i;
            break;
        }
    }
}

static void check_command_args(asm_config* asm_parameters)
{
    assert(asm_parameters);

    if (asm_parameters->reg[0] != '\0')
    {
        asm_parameters->masked_byte |= ARG_REG;
    }

    if ((strcmp(asm_parameters->command, "push") == 0) || CHECK_JUMP_COMMANDS)
    {
        if (!is_equal(asm_parameters->value, VALUE_GARBAGE))
        {
            asm_parameters->masked_byte |= ARG_IMMED;
        }
    }
}

static void turn_into_bytecode(asm_config* asm_parameters)
{
    assert(asm_parameters);

    if (asm_parameters->command[0] == '\0')
    {
        return;
    }

    *asm_parameters->code++ = asm_parameters->masked_byte;

    if (strcmp(asm_parameters->command, "push") == 0)
    {
        if (asm_parameters->reg[0] == '\0')
        {
            memcpy((double*) asm_parameters->code, &asm_parameters->value, sizeof(double)); asm_parameters->code += sizeof(double);
            return;
        }

        if (!is_equal(asm_parameters->value, VALUE_GARBAGE))
        {
            *asm_parameters->code++ = asm_parameters->reg_code;
            memcpy((double*) asm_parameters->code, &asm_parameters->value, sizeof(double)); asm_parameters->code += sizeof(double);
            return;
        }
    }

    if (CHECK_JUMP_COMMANDS)
    {
        if (is_equal(asm_parameters->value, NAMED_LABEL))
        {
            char* label_ptr = find_named_label(asm_parameters);
            double offset = asm_parameters->label - label_ptr + 1;
            memcpy((double*) asm_parameters->code, &offset, sizeof(double)); asm_parameters->code += sizeof(double);
            return;
        }

        if ((asm_parameters->labels[(int) asm_parameters->value] == 0) &&
            (asm_parameters->position == nullptr))
        {
            *((double*) asm_parameters->code) = -1;
                        asm_parameters->code += sizeof(double);
            return;
        }

        *((double*) asm_parameters->code) = (double) (asm_parameters->label - asm_parameters->labels[(int) asm_parameters->value] + 1);
                    asm_parameters->code += sizeof(double);
        return;
    }

    if (asm_parameters->reg[0] != '\0')
    {
        *asm_parameters->code++ = asm_parameters->reg_code;
        return;
    }
}

static char* find_label(asm_config* asm_parameters)
{
    assert(asm_parameters->buffer);

    int line = 0;

    for (int i = 0; (asm_parameters->buffer[i] != '\n') && (asm_parameters->buffer[i] != '\0'); i++)
    {
        if (asm_parameters->buffer[i] == ':')
        {
            sscanf(&asm_parameters->buffer[i] + 1, "%d", &line);
            asm_parameters->labels[line] = asm_parameters->code;

            return asm_parameters->code;
        }
    }

    return nullptr;
}

static void roll_parameters_back(asm_config* asm_parameters)
{
    assert(asm_parameters);

    asm_parameters->reg[0]   = '\0';
    asm_parameters->value    = VALUE_GARBAGE;
    asm_parameters->position = nullptr;
    asm_parameters->masked_byte = 0;
}
