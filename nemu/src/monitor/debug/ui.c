#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int info(char *args) {
    if (args == NULL) {
        printf("No such operation.\n");
    }
    else if (strcmp(args, "r") == 0) {
        printf("eax %x %d\n", cpu.eax, cpu.eax);
        printf("ecx %x %d\n", cpu.ecx, cpu.ecx);
        printf("edx %x %d\n", cpu.edx, cpu.edx);
        printf("ebx %x %d\n", cpu.ebx, cpu.ebx);
        printf("esp %x %d\n", cpu.esp, cpu.esp);
        printf("ebp %x %d\n", cpu.ebp, cpu.ebp);
        printf("esi %x %d\n", cpu.esi, cpu.esi);
        printf("edi %x %d\n", cpu.edi, cpu.edi);
        printf("eip %x %d\n", cpu.eip, cpu.eip);
    }
    else if (strcmp(args, "w") == 0){
        show_watchpoint();
    }
    else {
        printf("No such operation.\n");
    }
    return 0;
}

static int step_i(char *args) {
    int step_ct;
    if (args == NULL) 
        step_ct = 1;
    else 
        step_ct = atoi(args);
    cpu_exec(step_ct);
    return 0;
}

static int print_expr(char *args) {
    bool success[2];
    int ret_v = expr(args, success);
    if (*success == false) 
        printf("Unvaliable expression.\n");
    else
        printf("%s: %d\n", args, ret_v);
    return 0;
}

static int show_mem(char *args) {

    int n = 0, i;
    for ( ; *args == ' '; ++args);
    for ( ; *args >= '0' && *args <= '9'; ++args)
        n = n * 10 + *args - '0';
    puts("show mem: ");

    bool success[2];
    int start_pos = expr(args, success);
    if (*success == false) {
        printf("Unvaliable expression.\n");
        return -1;
    }

    for (i = 0; i < n; ++i)
        printf("%x\n", hwaddr_read(start_pos + i*4, 4));

    return 0;
}

typedef struct {
    swaddr_t prev_ebp;
    swaddr_t ret_addr;
    uint32_t args[4];
} PartOfStackFrame;


int find_func_name(int eip) {
    for (int i = 0; i < nr_symtab_entry; ++i)
        if ((symtab[i].st_info & STT_FUNC) && eip >= symtab[i].st_value && eip <= symtab[i].st_value + symtab[i].st_size)
            return symtab[i].st_name;
    Assert(0, "funcion not find");
}


static int print_stack_frame(char* args) {
    int ebp = cpu.ebp;
    printf("ebp: 0x%x, funcname: %s\n", ebp, strtab + find_func_name(cpu.eip));
    while (ebp != 0) {
        PartOfStackFrame sf;
        sf.prev_ebp = swaddr_read(ebp, 4);
        sf.ret_addr = swaddr_read(ebp+4, 4);
        printf("prev_ebp: 0x%x, ret_addr: 0x%x\n", sf.prev_ebp, sf.ret_addr);
        for (int i = 0; i < 4; ++i) {
            if (ebp+12+i*4 <= 0x8000000)
                sf.args[i] = swaddr_read(ebp+8+i*4, 4);
            else
                sf.args[i] = 0;
            printf("args[%d]: 0x%x, ", i, sf.args[i]);
        }
        puts("\n");
        ebp = sf.prev_ebp;
    }
    return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
    { "si", "step x operations", step_i},
    { "info", "Show all registers", info},
    { "p", "print expression value", print_expr},
    { "x", "Show memory", show_mem},
    { "w", "create watch point", create_watchpoint},
    { "d", "delete watch point", delete_watchpoint},
    { "bt", "print stack frame", print_stack_frame},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
