#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
    OPERAND_W(op_dest, op_dest->val - op_src->val);
    print_asm_template2();
}

// 2C 2D
make_instr_helper(i2r)
// 80 81 83
make_instr_helper(i2rm)
// 28 29
make_instr_helper(r2rm)
// 2A 2B
make_instr_helper(rm2r)



#include "cpu/exec/template-end.h"
