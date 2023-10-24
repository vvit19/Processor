#include <cstdio>
#include <cstdlib>

#include "asm.h"
#include "utils.h"

int main(int argc, const char** argv)
{
    asm_config asm_parameters = {};

    run_asm(&asm_parameters, argv[1], argv[2]);

    return 0;
}
