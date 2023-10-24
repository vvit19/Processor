#include <cassert>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdint>

#include "spu.h"
#include "stack.h"
#include "utils.h"

#define RETURN(exit_code) \
    if (exit_code == EXIT) \
    {                       \
        print_circle(proc);  \
        free(buffer);         \
        return exit_code;      \
    }

static return_code do_command    (spu* proc);
static elem_t*     check_register(spu* proc);
static elem_t*     get_argument  (spu* proc);
static void        print_circle  (spu* proc);

return_code spu_start(spu* proc, const char* byte_file)
{
    assert(proc);
    assert(byte_file);

    return_code exit_code = CONTINUE;

    char* buffer = get_file_content(byte_file);

    stack stk = {};
    proc->stk = &stk;
    STACK_CTOR(proc->stk);

    proc->ram[100] = {};

    proc->code = buffer;
    while (*proc->code != EOF && *proc->code != '\0')
    {
        proc->command = *proc->code++;
        exit_code     = do_command(proc);

        // stack_dump(proc->stk);
        RETURN(exit_code);
    }

    RETURN(EXIT);
}

static return_code do_command(spu* proc)
{
    assert(proc);

    elem_t temp_1 = 0, temp_2 = 0;

    #define DEF_CMD(string_name, name, code)    \
        case name:                               \
            code                                  \
            break;

    switch(proc->command & COMMAND)
    {
        #include "commands.h"

        default:
            fprintf(stderr, "Unknown case file: %s, line: %d\n", __FILE__, __LINE__);
            break;
    }

    #undef DEF_CMD

    return CONTINUE;
}

static elem_t* get_argument(spu* proc)
{
    assert(proc);

    elem_t* reg_ptr = nullptr;

    if ((proc->command & ARG_IMMED) &&
        (proc->command & ARG_REG))
    {
        proc->reg_num = *proc->code++;
        reg_ptr       = check_register(proc);
        proc->value   = *((elem_t*) proc->code); proc->code += sizeof(elem_t);
        proc->value  += *reg_ptr;

        if (proc->command & ARG_MEM)
        {
            return (elem_t*) &proc->ram[(int) proc->value];
        }

        return &proc->value;
    }

    if ((proc->command & ARG_IMMED) != 0)
    {
        memcpy(&proc->value, (elem_t*) proc->code, sizeof(elem_t)); proc->code += sizeof(elem_t);

        if (proc->command & ARG_MEM)
        {
            return (elem_t*) &proc->ram[(int) proc->value];
        }

        return &proc->value;
    }

    if ((proc->command & ARG_REG) != 0)
    {
        proc->reg_num = *proc->code++;
        reg_ptr       = check_register(proc);

        if (proc->command & ARG_MEM)
        {
            return (elem_t*) &proc->ram[(int) *reg_ptr];
        }
    }

    return reg_ptr;
}

static elem_t* check_register(spu* proc)
{
    assert(proc);

    switch ((int) proc->reg_num)
    {
    case 0:
        return &proc->reg.rax;
    case 1:
        return &proc->reg.rbx;
    case 2:
        return &proc->reg.rcx;
    case 3:
        return &proc->reg.rdx;
    default:
        fprintf(stderr, "Unknown register: %s, line: %d\n", __FILE__, __LINE__);
        break;
    }

    return nullptr;
}

static void print_circle(spu* proc)
{
    assert(proc);

    for (int i = 0; i < RAM_SIZE; i++)
    {
        if (proc->ram[i] != 0)
        {
            printf("%d\n", proc->ram[i]);
            printf(" * ");
        }
        else
        {
            printf(" . ");
        }

        if (( (i + 1) % (int) sqrt(RAM_SIZE) == 0) && (i != 0))
        {
            putchar('\n');
        }
    }
}
