#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr jle

make_helper(concat(jle_rel_, SUFFIX)) {
    DATA_TYPE rel = instr_fetch(cpu.eip + 1, DATA_BYTE);
    int src = (int)((DATA_TYPE_S)rel);

    if (GET_ZF && GET_SF != GET_OF) {
        cpu.eip += src;
        if (ops_decoded.is_data_size_16)
            cpu.eip = cpu.eip& 0xffff;
    }
    print_asm_template1();

    return DATA_BYTE + 1;
}

#include "cpu/exec/template-end.h"
