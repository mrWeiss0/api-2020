#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
 #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
 #define debug(...)
#endif

/* readline buffer */
#define BUF_SZ 0x402
#define NEWL '\n'
char buf[BUF_SZ];

/* cmd arguments */
#define ARGN 2
#define ARGSEP ','
unsigned int args[ARGN];
int quit = 0;

/* commands */
typedef void cmd_ft(void);
char    const cmd_ch[] = "c"       "d"       "p"
                         "u"       "r"       "q";
cmd_ft                    cmd_fn_c, cmd_fn_d, cmd_fn_p,
                          cmd_fn_u, cmd_fn_r, cmd_fn_q;
cmd_ft *const cmd_fn[] = {cmd_fn_c, cmd_fn_d, cmd_fn_p,
                          cmd_fn_u, cmd_fn_r, cmd_fn_q};

/* save state struct */
struct savest {
	struct savest *link [2];
	size_t         touch[2];
	size_t         len;
	char          *line [ ];
};

struct savest *root, *head;

/* main */
void readline();
void cmd_parse();
struct savest *new_savest(size_t len);
void del_savest_all(struct savest *state);

int main(){
	root = head = new_savest(0);
	while(!quit){
		readline();
		if(quit)
			break;
		cmd_parse();
	}
	del_savest_all(root);
	return 0;
}

/* parser */
void readline(){
	if(!fgets(buf, sizeof(buf), stdin)){
		quit = 1;
		return;
	}
	size_t bufl = strlen(buf);
	if(buf[bufl - 1] == NEWL)
		buf[--bufl] = '\0';
}

void cmd_err(char cmd){
	debug("no command %c\n", cmd);
}

void cmd_call(char cmd){
	char *ofs = strchr(cmd_ch, cmd);
	if(!ofs || !*ofs)
		cmd_err(cmd);
	else
		cmd_fn[ofs - cmd_ch]();
}

void cmd_parse(){
	char *p = buf;
	unsigned char i = 0;
	while(1){
		args[i++] = strtol(p, &p, 10);
		if(*p != ARGSEP)
			break;
		++p;
	}
	cmd_call(*p);
}

/* save state */
struct savest *new_savest(size_t len){
	struct savest *state = malloc(sizeof(*state) + len * sizeof(*state->line));
	if(!state)
		return NULL;
	memset(state, 0, sizeof(state->link) + sizeof(state->touch));
	state->len = len;
	return state;
}

void del_savest(struct savest *state){
	for(size_t i = state->touch[0]; i < state->touch[1]; ++i)
		free(state->line[i]);
	free(state);
}

void del_savest_all(struct savest *state){
	struct savest *temp;
	while(state){
		temp = state->link[1];
		del_savest(state);
		state = temp;
	}
}

void commit(struct savest *state){
	del_savest_all(head->link[1]);
	state->link[0] = head;
	head = head->link[1] = state;
}

/* commands definition */
void cmd_fn_c(){
	debug("change from %d to %d\n", args[0], args[1]);
}

void cmd_fn_d(){
	debug("delete from %d to %d\n", args[0], args[1]);
}

void cmd_fn_p(){
	debug("print  from %d to %d\n", args[0], args[1]);
}

void cmd_fn_u(){
	unsigned int i;
	for(i = 0; i < args[0] && head->link[0]; i++)
		head = head->link[0];
	debug("undo %d (%d)\n", i, args[0]);
}

void cmd_fn_r(){
	unsigned int i;
	for(i = 0; i < args[0] && head->link[1]; i++)
		head = head->link[1];
	debug("redo %d (%d)\n", i, args[0]);
}

void cmd_fn_q(){
	quit = 1;
	debug("quit\n");
}

