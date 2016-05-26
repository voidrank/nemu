#include "cpu/exec/helper.h"

#if DATA_BYTE == 1

#define SUFFIX b
#define DATA_TYPE uint8_t
#define DATA_TYPE_S int8_t

#elif DATA_BYTE == 2

#define SUFFIX w
#define DATA_TYPE uint16_t
#define DATA_TYPE_S int16_t

#elif DATA_BYTE == 4

#define SUFFIX l
#define DATA_TYPE uint32_t
#define DATA_TYPE_S int32_t

#else

#error unknown DATA_BYTE

#endif

#define REG(index) concat(reg_, SUFFIX) (index)
#define REG_NAME(index) concat(regs, SUFFIX) [index]

#define MEM_R(addr) swaddr_read(addr, DATA_BYTE)
#define MEM_W(addr, data) swaddr_write(addr, DATA_BYTE, data)

#define OPERAND_W(op, src) concat(write_operand_, SUFFIX) (op, src)

#define MSB(n) ((DATA_TYPE)(n) >> ((DATA_BYTE << 3) - 1))

#define SUB_EFLAGS(result,dest,src,result8) \
    /* clear */\
    cpu.eflags &=  0x726; \
    /* OF */ \
    cpu.eflags |= usub_ok(dest, src) << 11; \
    /* SF */ \
    cpu.eflags |= ((result>>((DATA_BYTE<<3)-1))&1) << 7; \
    /* ZF */ \
    cpu.eflags |= (result==0) << 6; \
    /* AF */ \
    cpu.eflags |= ((dest&0xf) < (src&0xf)) << 4;\
    /* PF */ \
    result8 = (result&0x55) + ((result>>1)&0x55); \
    result8 = (result8&0x33) + ((result8>>2)&0x33); \
    result8 = (result8&0xf) + ((result8>>4)&0xf); \
    result8 = result & 1; \
    cpu.eflags |= result8 << 2; \
    /* CF */ \
    cpu.eflags |= ((dest&0xff) < (src&0xff)) << 0; 

#define TEST_EFLAGS(result,result8) \
    /* clear */ \
    cpu.eflags &= 0x726; \
    /* SF */ \
    cpu.eflags |= ((result>>((DATA_BYTE<<3)-1))&1) << 7; \
    /* ZF */ \
    cpu.eflags |= (result==0) << 6; \
    /* PF */ \
    result8 = (result&0x55) + ((result>>1)&0x55); \
    result8 = (result8&0x33) + ((result8>>2)&0x33); \
    result8 = (result8&0xf) + ((result8>>4)&0xf); \
    result8 = result & 1; \
    cpu.eflags |= result8 << 2; \

