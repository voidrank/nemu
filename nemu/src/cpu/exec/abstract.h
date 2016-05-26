#ifndef __ABSTRACT__H_
#define __ABSTRACT__H_

#if DATA_BYTE == 4
    #define push(x) \
        REG(R_ESP) -= 4; \
        MEM_W(REG(R_ESP), x)
#elif DATA_BYTE == 2 
    #define push(x) \
        REG(R_ESP) -= 2; \
        MEM_W(REG(R_ESP), x)
#endif 

#endif
