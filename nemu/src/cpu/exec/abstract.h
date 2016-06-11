#ifndef __ABSTRACT__H_
#define __ABSTRACT__H_

#define GET_OF ((cpu.eflags>>11)&1)
#define GET_SF ((cpu.eflags>>7)&1)
#define GET_ZF ((cpu.eflags>>6)&1)
#define GET_AF ((cpu.eflags>>4)&1)
#define GET_PF ((cpu.eflags>>2)&1)
#define GET_CF (cpu.eflags&1)

#endif
