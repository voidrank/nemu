#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr je

make_helper(concat(je_rel_, SUFFIX)) {
    printf("%u\n", DATA_BYTE);
    DATA_TYPE rel = instr_fetch(cpu.eip + 1, DATA_BYTE);
    int src = (int)((DATA_TYPE_S)rel);
    cpu.eip += src;
#if DATA_BYTE == 16
    src = src & 0xffff;
#endif
    print_asm_template1();
    return DATA_BYTE + 1;
}

#include "cpu/exec/template-end.h"
