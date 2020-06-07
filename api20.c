#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
 #define debug(...) fprintf(stderr, "DEBUG " __VA_ARGS__)
#else
 #define debug(...)
#endif
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

/* readline buffer */
#define BUF_SZ 0x402
#define NEWL '\n'
char buf[BUF_SZ];

/* cmd arguments */
#define ARGN 2
#define ARGSEP ','
typedef size_t arg_t;
arg_t args[ARGN];
int quit = 0;

/* commands */
#define EMPTY "."
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
	unsigned int i = 0;
	while(1){
		args[i++] = strtol(p, &p, 10);
		if(*p != ARGSEP)
			break;
		++p;
	}
	cmd_call(*p);
}

/* save state */
#define get_line(n) ((n) < head->len ? head->line[n] : EMPTY)

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
	struct savest *state = new_savest(max(head->len, args[1]));
	memcpy(state->line, head->line, (args[0] - 1) * sizeof(*state->line));
	if(state->len > args[1])
		memcpy(state->line + args[1], head->line + args[1], (state->len - args[1]) * sizeof(*state->line));
	arg_t i = state->touch[0] = args[0] - 1;
	while(1){
		readline();
		if(!strcmp(buf, EMPTY))
			break;
		state->line[i++] = strdup(buf);
	}
	state->touch[1] = i;
	commit(state);
	debug("change %ld to %ld\n", args[0], args[1]);
}

void cmd_fn_d(){
	if(!args[0])
		args[0] = 1;
	struct savest *state = new_savest(min(head->len, head->len - (min(head->len, args[1]) - args[0] + 1)));
	memcpy(state->line, head->line, min(state->len, args[0] - 1) * sizeof(*state->line));
	if(head->len > args[1])
		memcpy(state->line + args[0] - 1, head->line + args[1], (head->len - args[1]) * sizeof(*state->line));
	commit(state);
	debug("delete %ld to %ld\n", args[0], args[1]);
}

void cmd_fn_p(){
	debug("print  %ld to %ld\n", args[0], args[1]);
	for(arg_t i = args[0]; i <= args[1]; i++)
		puts(get_line(i - 1));
}

void cmd_fn_u(){
	arg_t i;
	for(i = 0; i < args[0] && head->link[0]; i++)
		head = head->link[0];
	debug("undo %ld (%ld)\n", i, args[0]);
}

void cmd_fn_r(){
	arg_t i;
	for(i = 0; i < args[0] && head->link[1]; i++)
		head = head->link[1];
	debug("redo %ld (%ld)\n", i, args[0]);
}

void cmd_fn_q(){
	quit = 1;
	debug("quit\n");
}
