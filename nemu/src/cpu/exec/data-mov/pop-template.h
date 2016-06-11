#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr pop

static void do_execute() {
    DATA_TYPE ret;
    pop(ret);
    OPERAND_W(op_src, ret);
    print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm);

#include "cpu/exec/template-end.h"
