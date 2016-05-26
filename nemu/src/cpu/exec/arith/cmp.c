#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmp-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmp-template.h"
#undef DATA_BYTE


// 2C 2D
make_helper_v(cmp_i2r);
// 80 81 83
make_helper_v(cmp_i2rm);
// 28 29
make_helper_v(cmp_rm2r);
// 2A 2B
make_helper_v(cmp_r2rm);
make_helper_v(cmp_i2rm_b);
