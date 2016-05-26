#include "cpu/exec/template-start.h"
#include "cpu/reg.h"
#include <stdio.h>


#define instr cmp

static void do_execute() {

    DATA_TYPE src, dest, result;
    int result8;
    src = (DATA_TYPE_S)op_src->val;
    dest = (DATA_TYPE_S)op_dest->val;
    result = dest - src;

    SUB_EFLAGS(result,dest,src,result8);

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
// 83
#if DATA_BYTE > 1
make_instr_helper(i2rm_b)
#endif



#include "cpu/exec/template-end.h"
