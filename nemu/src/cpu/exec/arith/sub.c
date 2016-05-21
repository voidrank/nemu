#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "sub-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "sub-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "sub-template.h"
#undef DATA_BYTE


// 2C 2D
make_helper_v(sub_i2r);
// 80 81 83
make_helper_v(sub_i2rm);
// 28 29
make_helper_v(sub_rm2r);
// 2A 2B
make_helper_v(sub_r2rm);

