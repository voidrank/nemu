#include "cpu/exec/template-start.h"
#include "../abstract.h"

#define instr ret

make_helper(concat(ret_near_, SUFFIX)) {
#if DATA_BYTE == 2
    uint16_t *pip = (uint16_t*)&cpu.eip;
    pop(*pip);
    cpu.eip = cpu.eip & 0xffff;
#elif DATA_BYTE == 4
    pop(cpu.eip);
#endif
    print_asm("ret");
    return 0;
}

#include "cpu/exec/template-end.h"
