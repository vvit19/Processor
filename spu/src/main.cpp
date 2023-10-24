#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include "spu.h"
#include "utils.h"
#include "stack.h"

#define RETURN(exit_code)         \
    if (exit_code == EXIT)         \
    {                               \
        stack_dtor(proc.stk);        \
        return exit_code;             \
    }

int main(int argc, const char** argv)
{
    spu proc = {};
    return_code exit_code = spu_start(&proc, argv[1]);
    RETURN(exit_code);
}
