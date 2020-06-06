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

void readline();
void cmd_parse();

int main(){
	while(!quit){
		readline();
		if(quit)
			break;
		cmd_parse();
	}
	return 0;
}

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
	debug("undo %d\n", args[0]);
}

void cmd_fn_r(){
	debug("redo %d\n", args[0]);
}

void cmd_fn_q(){
	quit = 1;
	debug("quit\n");
}

