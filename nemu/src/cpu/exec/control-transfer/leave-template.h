#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr leave

static void do_execute() {

#if DATA_BYTE == 2
    REG(R_SP) = REG(R_BP);
#elif DATA_BYTE == 4
    REG(R_ESP) = REG(R_EBP);
#endif

#if DATA_BYTE == 2
    pop(REG(R_BP));
#elif DATA_BYTE == 4
    pop(REG(R_EBP));
#endif
    
    print_asm("leave");
}


make_instr_helper(r);

#include "cpu/exec/template-end.h"
