#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
	NOTYPE = 256, EQ, PLUS, SUBTRACT, MULTIPLY, DIVIDE, POW,
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
    NUMBER, REG
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	{" +",	NOTYPE},				// spaces
	{"\\+", PLUS},					// plus
    {"\\-", SUBTRACT},              // subtract
    {"\\*", MULTIPLY},              // multiply
    {"\\/", DIVIDE},                // divide
    {"\\^", POW},                   // power
    {"[0-9]+", NUMBER},             // number
    {"\\(", LEFT_PARENTHESIS},        // left parenthesis
    {"\\)", RIGHT_PARENTHESIS},       // right parenthesis
    {"(%eax)|(%ecx)|(%edx)|(%ebx)|(%esp)|(%ebp)|(%esi)|(%edi)", REG}, // register
	{"==", EQ}						// equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;
    
                // record
                tokens[nr_token].type = rules[i].token_type;
                memset(tokens[nr_token].str, 0, sizeof(tokens[nr_token].str));
                memcpy(tokens[nr_token].str, e + position - substr_len, sizeof(char) * substr_len);
                ++nr_token;

                /*
				switch(rules[i].token_type) {
					default: panic("please implement me");
				}
                */

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}


int num_stack[32];
int opt_stack[32];
int num_ct, opt_ct;


static int cal_pri(int opt) {
    if (opt == PLUS || opt == SUBTRACT)
        return 1;
    else if (opt == MULTIPLY || opt == DIVIDE)
        return 2;
    else if (opt == POW)
        return 3;
    else
        return 0x7f;
}

#define exp_opt(opt) num_stack[num_ct-2] = num_stack[num_ct-2] opt num_stack[num_ct-1]
#define exp_eq(opt) opt_stack[opt_ct-1]==opt

static inline void opt_top() {
    if (exp_eq(PLUS))
        exp_opt(+);
    else if (exp_eq(SUBTRACT))
        exp_opt(-);
    else if (exp_eq(MULTIPLY))
        exp_opt(*);
    else if (exp_eq(DIVIDE))
        exp_opt(/);
    --opt_ct;
    --num_ct;
}

static inline void push_num(int num) {
    num_stack[num_ct++] = num;
}

static inline void push_opt(int opt) {
    opt_stack[opt_ct++] = opt;
}

static inline void pop_opt() {
    opt_ct--;
}

static inline void pop_num() {
    num_ct--;
}

static inline int get_opt_top() {
    return opt_stack[opt_ct-1];
}

static inline int get_num_top() {
    return num_stack[num_ct-1];
}


static inline int get_register(char *name) {
    if (strcmp(name, "%eax") == 0)
        return cpu.eax;
    else if (strcmp(name, "%ecx") == 0)
        return cpu.ecx;
    else if (strcmp(name, "%edx") == 0)
        return cpu.edx;
    else if (strcmp(name, "%ebx") == 0)
        return cpu.ebx;
    else if (strcmp(name, "%esp") == 0)
        return cpu.esp;
    else if (strcmp(name, "%ebp") == 0)
        return cpu.ebp;
    else if (strcmp(name, "%esi") == 0)
        return cpu.esi;
    else if (strcmp(name, "%edi") == 0)
        return cpu.edi;
    else if (strcmp(name, "%eip") == 0)
        return cpu.eip;
    return 0;
}


uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}


    opt_ct = 0;
    num_ct = 0;

    int i;
    for (i = 0; i < nr_token; ++i) {
        // constant
        if (tokens[i].type == NUMBER) {
            push_num(atoi(tokens[i].str));
        }
        // nornmal operation
        else if (tokens[i].type >= EQ && tokens[i].type <= POW) {
            int now_pri = cal_pri(tokens[i].type);
            while ( opt_ct > 0 && num_ct > 0 && get_opt_top() != LEFT_PARENTHESIS && 
                cal_pri(opt_stack[opt_ct-1]) >= now_pri)
                opt_top();
            push_opt(tokens[i].type);
        }
        // left parenthesis
        else if (tokens[i].type == LEFT_PARENTHESIS) {
            push_opt(tokens[i].type);
        }
        // right parenthesis
        else if (tokens[i].type == RIGHT_PARENTHESIS) {
            while (opt_ct > 0 && num_ct > 0 && opt_stack[opt_ct-1] != LEFT_PARENTHESIS) {
                opt_top();
            }

            if (opt_ct == 0 || opt_stack[opt_ct-1] != LEFT_PARENTHESIS) {
                *success = false;
                return -1;
            }

            pop_opt();
        }
        // REG
        else if (tokens[i].type == REG) {
            push_num(get_register(tokens[i].str));
        }
    }

    while (opt_ct > 0) {
        if (num_ct <= 1) {
            *success = false;
            return -1;
        }
        opt_top();
    }

    //printf("%d %d\n", opt_ct, num_ct);

    if (num_ct > 1) {
        *success = false;
        return -1;
    }

    *success = true;

	return num_stack[0];
}

