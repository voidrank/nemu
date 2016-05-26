#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr push

static void do_execute() {
    print_asm_template1();
    push(op_src->val);
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
