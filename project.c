#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30

void shell_exit(char **args)
{
    exit(0);
}

void shell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "shell69: missing argument\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
	{
            perror("shell69: cd");
        }
    }
}

void shell_help(char **args)
{
    char *helptext =
        "Shell69\n"
        "The following commands are available:\n"
        "  cd       Change the working directory.\n"
        "  exit     Exit the shell.\n"
        "  help     Print this help text.\n";
    printf("%s", helptext);
}

struct builtin
{
    char *name;
    void (*func)(char **args);
};

struct builtin builtins[] = {
    {"help", shell_help},
    {"exit", shell_exit},
    {"cd", shell_cd},
};

int shell_num_builtins() {
    return sizeof(builtins) / sizeof(struct builtin);
}

char* read_cmd(char *prompt, FILE* fp)
{
	printf("%s", prompt);
	char *cmdline = (char*)malloc(sizeof(char)*MAX_LEN);
	int c;
	int pos = 0;
	while((c = getc(fp)) != EOF)
	{
		if(c == '\n')
			break;
		cmdline[pos++] = c;
	}
	cmdline[pos] = '\0';
	return cmdline;
}
char** tokenize(char *cmdline)
{
	char **arglist = (char**)malloc(sizeof(char*)*MAXARGS+1);
	for(int i = 0; i < MAXARGS+1; i++)
	{
		arglist[i] = (char*)malloc(sizeof(char)*ARGLEN);
		bzero(arglist[i], ARGLEN);
	}
	char *cp = cmdline;
	char *start;
	int len, argnum = 0;
	while(*cp != '\0')
	{
		while(*cp == ' ' || *cp == '\t')
			cp++;
		start = cp;
		len = 1;
		while(*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
			len++;
		strncpy(arglist[argnum], start, len);
		arglist[argnum][len] = '\0';
		argnum++;
	}
	arglist[argnum] = NULL;
	return arglist;
}
int execute(char *arglist[])
{
	for (int i = 0; i < shell_num_builtins(); i++) {
        	if (strcmp(arglist[0], builtins[i].name) == 0) {
            builtins[i].func(arglist);
            return 0;
        }
	}
	int status;
	int cpid = fork();
	if(cpid == -1)
	{
	 	perror("fork failed!");
		exit(1);
	}
	else if(cpid == 0) {
		
		execvp(arglist[0], arglist);
		perror("execvp failed!");
		exit(2);
	}
	else {
		while(wait(&status) != cpid);
		printf("Child exited with status %d\n", status >> 8);
		return 0;
	}
}
int main()
{
	char *prompt = "shell69: ";
	char *cmdline;
	char **arglist;
	while((cmdline = read_cmd(prompt, stdin)) != NULL)
	{
		if((arglist = tokenize(cmdline)) != NULL)
		{
			execute(arglist);
			for(int i = 0; i < MAXARGS+1; i++)
				free(arglist[i]);
			free(arglist);
			free(cmdline);
		}	
	}
	printf("\n");
	return 0;
}
