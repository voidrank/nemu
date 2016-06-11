#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr jmp

make_helper(concat(jmp_rel_, SUFFIX)) {
    DATA_TYPE rel = instr_fetch(cpu.eip + 1, DATA_BYTE);
    cpu.eip += rel;
    if (ops_decoded.is_data_size_16)
        cpu.eip = cpu.eip & 0xffff;
    print_asm("jmp" str(SUFFIX) " 0x%x", rel);
    return 1 + DATA_BYTE;
}

#include "cpu/exec/template-end.h"
