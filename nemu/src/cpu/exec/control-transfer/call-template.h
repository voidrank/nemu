#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr call

make_helper(concat(call_rel_, SUFFIX)) {
    DATA_TYPE rel = instr_fetch(cpu.eip + 1, DATA_BYTE);
    push(cpu.eip);
    cpu.eip += rel;
    print_asm_template1();
    return 5;
}

#include "cpu/exec/template-end.h"
