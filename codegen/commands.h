#ifndef COMMANDS_H
#define COMMANDS_H

#define POP_VALUES                  \
    stack_pop(proc->stk, &temp_1);   \
    stack_pop(proc->stk, &temp_2);    \

#define POP_VALUE                   \
    stack_pop(proc->stk, &temp_1);   \

DEF_CMD("hlt", HLT,
{
    return EXIT;
})

DEF_CMD("out", OUT,
{
    stack_pop(proc->stk, &proc->value);
    printf("RESULT = %lf\n", proc->value);
})

DEF_CMD("outc", OUTC,
{
    stack_pop(proc->stk, &proc->value);
    printf("%c", (int) proc->value);
})

DEF_CMD("in", IN,
{
    printf("Push: ");
    scanf("%lf", &proc->value);
    stack_push(proc->stk, proc->value);
})

DEF_CMD("push", PUSH,
{
    if (proc->command & ARG_MEM)
    {
        char* popped_value = (char*) get_argument(proc);
        stack_push(proc->stk, (elem_t) *popped_value);
    }
    else
    {
        stack_push(proc->stk, *get_argument(proc));
    }
})

DEF_CMD("pop", POP,
{
    if (proc->command & ARG_MEM)
    {
        char* ram_ptr = (char*) get_argument(proc);
        stack_pop(proc->stk, &proc->value);
        *ram_ptr = (char) proc->value;
    }
    else
    {
        stack_pop(proc->stk, get_argument(proc));
    }
})

DEF_CMD("add", ADD,
{
    POP_VALUES
    stack_push(proc->stk, temp_1 + temp_2);
})

DEF_CMD("sub", SUB,
{
    POP_VALUES
    stack_push(proc->stk, temp_2 - temp_1);
})

DEF_CMD("mult", MULT,
{
    POP_VALUES
    stack_push(proc->stk, temp_1 * temp_2);
})

DEF_CMD("div", DIV,
{
    POP_VALUES
    stack_push(proc->stk, temp_2 / temp_1);
})

DEF_CMD("sin", SIN,
{
    POP_VALUE
    stack_push(proc->stk, sin(temp_1));
})

DEF_CMD("cos", COS,
{
    POP_VALUE
    stack_push(proc->stk, cos(temp_1));
})

DEF_CMD("sqrt", SQRT,
{
    POP_VALUE
    stack_push(proc->stk, sqrt(temp_1));
})

DEF_CMD("jump", JMP,
{
    proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
})

DEF_CMD("ja", JA,
{
    POP_VALUES

    if (temp_1 > temp_2)
    {
        proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
    }
    else
    {
        proc->code += sizeof(elem_t);
    }
})

DEF_CMD("jae", JAE,
{
    POP_VALUES

    if ((temp_1 > temp_2) || is_equal(temp_1, temp_2))
    {
        proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
    }
    else
    {
        proc->code += sizeof(elem_t);
    }
})

DEF_CMD("jb", JB,
{
    POP_VALUES

    if (temp_1 < temp_2)
    {
        proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
    }
    else
    {
        proc->code += sizeof(elem_t);
    }
})

DEF_CMD("jbe", JBE,
{
    POP_VALUES

    if ((temp_1 < temp_2) || is_equal(temp_1, temp_2))
    {
        proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
    }
    else
    {
        proc->code += sizeof(elem_t);
    }
})

DEF_CMD("je", JE,
{
    POP_VALUES

    if (is_equal(temp_1, temp_2))
    {
        proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
    }
    else
    {
        proc->code += sizeof(elem_t);
    }
})

DEF_CMD("jne", JNE,
{
    POP_VALUES

    if (!is_equal(temp_1, temp_2))
    {
        proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
    }
    else
    {
        proc->code += sizeof(elem_t);
    }
})

DEF_CMD("call", CALL,
{
    stack_push(proc->stk, (double) ( (proc->code + sizeof(elem_t)) - (char*) 0 ));
    proc->code -= ( ( (int) *get_argument(proc) ) + sizeof(elem_t) );
})

DEF_CMD("ret", RET,
{
    double position = 0;
    stack_pop(proc->stk, &position);
    proc->code = (char*) ((size_t) position);
})

#endif
