#include <cstdlib>

#include "disasm.h"
#include "utils.h"

int main(int argc, const char** argv)
{
    disasm_config disasm = {};
    disasm_text(&disasm, argv[1], argv[2]);

    return 0;
}
