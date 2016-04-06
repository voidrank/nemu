#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define EXPR_MAX_LEN 128

typedef struct watchpoint {
	int NO, value;
    bool valid;
	struct watchpoint *next;
    char expr[EXPR_MAX_LEN];

} WP;

void show_watchpoint();
int create_watchpoint(char *);
int delete_watchpoint(char *);
bool check_change();


#endif
