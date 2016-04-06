#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#include <stdlib.h>

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}


void show_watchpoint() {
    bool success[2];
    for (WP *i = head; i != NULL; i = i->next) {
        memset(success, 0, sizeof success);
        int retv = expr(i->expr, success);
        if (*success == false)
            printf("watch point NO.%d: %s cause an error", i->NO, i->expr);
        else
            printf("watch point NO.%d, %s value: %d.\n", i->NO, i->expr, retv); 
    }
}

int create_watchpoint(char *args) {
    if (free_ == NULL) {
        printf("No availiable mem for allocate watch point");
        return -1;
    }
    else {
        WP *new_ = free_;
        free_ = free_->next;

        new_->next = head;
        memcpy(new_->expr, args, sizeof(args[0]) * strlen(args));
        head = new_;
        bool success[2];
        head->value = expr(args, success);
        head->valid = *success;
        show_watchpoint();

        return 0;
    }
}

int delete_watchpoint(char *args) {
    int no = atoi(args);
    WP *last = NULL;
    bool found = 0;
    for (WP *now = head; now != NULL; now = now->next) {
        if (now->NO == no) {
            if (last == NULL) {
                head = now->next;
            }
            else {
                last->next = now->next;
            }
            now->next = free_;
            free_ = now;
            memset(free_, 0, sizeof(*free_));
            found = 1;
        }
        last = now;
    }
    if (!found)
        printf("Not found.\n");
    return 0;
}

bool check_change() {
    bool found = 0;
    for (WP *now = head; now != NULL; now = now->next) {
        bool success[2];
        int value = expr(now->expr, success);
        if (*success == false) {
            if (now->valid == true)
                found = 1;
        }
        else {
            if (now->value != value)
                found = 1;
        }
        now->valid = *success;
        now->value = value;
    }

    return found;
}

