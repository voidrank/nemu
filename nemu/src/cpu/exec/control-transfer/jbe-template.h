#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr jbe

make_helper(concat(jbe_rel_, SUFFIX)) {
    int rel = (int)((DATA_TYPE_S)instr_fetch(cpu.eip + 1, DATA_BYTE));
    if (GET_CF || GET_ZF) {
        cpu.eip += rel;
        if (ops_decoded.is_data_size_16)
            cpu.eip = cpu.eip & 0xffff;
    }
    print_asm("jbe" str(SUFFIX) " 0x%x", rel);
    return 1 + DATA_BYTE;
}

#include "cpu/exec/template-end.h"
