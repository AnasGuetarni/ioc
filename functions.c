/*
    Title       :   Interpreteur for unix command (shell_functions.c)
    Author      :   Erias Diego & Pisanello Antonio
    Date        :   17/11/17
    Description :   The functions for the terminal commands

   ln                 : creates a shortcut (link) for a filename
   char *mainfilename : name of the file that you want to create a shortcut
   char *linkfile     : name of the shortcut
 */

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <regex.h>

#include "functions.h"

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
        "cd",
        "help",
        "exit",
        "pwd",
        "alias"
};

int (*builtin_func[]) (char **) = {
        &ioc_cd,
        &ioc_help,
        &ioc_exit,
        &ioc_pwd,
        &ioc_alias
};

int ioc_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */

int ioc_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "ioc: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("ioc");
        }
    }
    return 1;
}

/**
   @brief search the pos of an array of strings.
   @return the positions, either it stop the program while returning -1.
 */

int strpos(char *haystack, char *needle)
{
  char *p = strstr(haystack, needle);
  if (p)
     return p - haystack;
  return -1;   // Not found = -1.
}

#define IOC_PATH_BUFSIZE 1024

/**
   @brief Builtin command: get path.
   @param char * filename : the name of the file who contains the progiles.
   @return Always returns 1, to continue executing.
 */

int get_path(char *filename){
  char *line = NULL;
	uint8_t nbRegex = 2;
	const char *regexS[nbRegex];
	regexS[0] = "([_|[:alpha:]]+[[:alnum:]|_]*)=[\"|']+([[:alnum:]|[:print:]]+)[\"|']+";
	regexS[1] = "([_|[:alpha:]]+[[:alnum:]|_]*)=([[:alnum:]|[:graph:]]+)";
	size_t groups = 3;
	regex_t compiled_regex;
	regmatch_t groupArray[groups];
	FILE *fp;
	ssize_t read;
	size_t len = 0;
	int line_count = 0;
	int pos;

	fp = fopen(filename, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	while ((read = getline(&line, &len, fp)) != -1) {
		line_count++;
		for(int i=0; i < nbRegex; i++){
			if (regcomp(&compiled_regex, regexS[i], REG_EXTENDED)){
				printf("Could not compile regular expression.\n");
			}
			if (regexec(&compiled_regex, line, groups, groupArray, 0) == 0){
				char* env_name = calloc(groupArray[1].rm_eo - groupArray[1].rm_so + 2, sizeof(char));
				char* env_val  = calloc(groupArray[2].rm_eo - groupArray[2].rm_so + 2, sizeof(char));
				strncpy(env_name, line, groupArray[1].rm_eo);
				strncpy(env_val , line + groupArray[2].rm_so, groupArray[2].rm_eo - groupArray[2].rm_so);
				if((i == 1)&((env_val[strlen(env_val) - 1] == '"')|(env_val[strlen(env_val) - 1] == '\'')))
					break;
				setenv(env_name, env_val, 1);
				break;
			}
		}
  }

  fclose(fp);
  if (line)
  free(line);

  regfree(&compiled_regex);
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */

int ioc_help(char **args)
{
    int i;
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < ioc_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int ioc_exit(char **args) {
    return 0;
}

#define IOC_PWD_BUFSIZE 1024

/**
   @brief Builtin command: pwd.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int ioc_pwd(char **args){
  char cwd[IOC_PWD_BUFSIZE];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "%s\n", cwd);
   else
       perror("getcwd() error");
   return 1;
}

/**
   @brief Builtin command: ln -> create a link between two files
   @param args List of args.
   @return Always returns 1, to continue execution.
 */
int ioc_alias(char **args){
  char *mainfile, *linkfile;
  mainfile = args[1];
  linkfile = args[2];
  link(mainfile, linkfile);
  return 1;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */

int ioc_launch(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("ioc");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("ioc");
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int ioc_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < ioc_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return ioc_launch(args);
}

#define IOC_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *ioc_read_line(void)
{
    int bufsize = IOC_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "ioc: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += IOC_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "ioc: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define IOC_TOK_BUFSIZE 64
#define IOC_TOK_DELIM " \t\r\n\a"
/**
   @brief Loop getting input and executing it.
 */
void ioc_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = ioc_read_line();
        args = ioc_split_line(line);
        status = ioc_execute(args);

        free(line);
        free(args);
    } while (status);
}

/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **ioc_split_line(char *line)
{
    int bufsize = IOC_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        fprintf(stderr, "ioc: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, IOC_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += IOC_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "ioc: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, IOC_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
