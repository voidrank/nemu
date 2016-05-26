#include "cpu/exec/template-start.h"
#include "cpu/reg.h"
#include <stdio.h>


#define instr test

static void do_execute() {

    DATA_TYPE src, dest, result;
    int result8;
    src = op_src->val;
    dest = op_dest->val;
    result = dest & src;

    TEST_EFLAGS(result,result8);

    OPERAND_W(op_dest, result);
    print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)



#include "cpu/exec/template-end.h"
