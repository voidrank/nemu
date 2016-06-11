#include "cpu/exec/template-start.h"
#include "cpu/reg.h"
#include <stdio.h>


#define instr sub

static void do_execute() {

    DATA_TYPE src, dest, result;
    int result8;
    if (op_src->size == 1)
        src= (DATA_TYPE_S)((char)op_src->val);
    else
        src = op_src->val;

    dest = op_dest->val;
    result = dest - src;

    SUB_EFLAGS(result,dest,src,result8);

    OPERAND_W(op_dest, dest - src);
    print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
#if DATA_BYTE > 1
make_instr_helper(i2rm_b)
#endif



#include "cpu/exec/template-end.h"
