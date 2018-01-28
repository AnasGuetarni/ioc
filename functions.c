/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		functions.c
 Project:	ioc
 *****************************/

#include "functions.h"

char *progs[NB_PROGS];
alias_t alias[MAX_ALIAS];
int countAlias;

/**
 * Initialize intern programs
 */
void initProg(){
	progs[0] = "pwd";
	progs[1] = "cd";
	progs[2] = "exit";
	progs[3] = "help";
	progs[4] = "echo";
	progs[5] = "alias";
}

/**
 * Check if its an external program
 * @param inputArgs: list of arguments from the command line
 * @param size: arguments size
 * @param copyInput: copy of the command line
 * @return return 1
 */
int isCommande(char **inputArgs, int size, char *copyInput){
	char *path;

	//Get path
	if((path = getPath(inputArgs[0])) != NULL){

		//Replace first argument
		inputArgs[0] = path;

		//Execute program
		execute(inputArgs, 0, NULL);
		return 1;
	}
	return 0;
}

/**
 * Check if its an internal program
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 * @param copyInput: copy of the command line
 * @return if we found the command return 1
 */
int isProg(char **inputArgs, int size, char *copyInput){
	int i;
	for (i = 0; i < NB_PROGS; ++i) {
		if(!strcmp(progs[i], inputArgs[0])){

			//Check command
			if(!strcmp(progs[i], "pwd")){
				if(size == 1){
					pwdCmd(inputArgs, size);
				}else{
					helpPwd();
				}
			}else if(!strcmp(progs[i], "cd")){
				if(size == 2){
					cdCmd(inputArgs, size);
				}else{
					helpCd();
				}
			}else if(!strcmp(progs[i], "exit")){
				if(size == 2){
					exitCmd(inputArgs, size);
				}else{
					helpExit();
				}
			}else if(!strcmp(progs[i], "help")){
				if(size == 2){
					helpCmd(inputArgs, size);
				}else{
					helpHelp();
				}
			}else if(!strcmp(progs[i], "echo")){
				if(size == 2){
					echoCmd(inputArgs, size);
				}else{
					helpEcho();
				}
			}else if(!strcmp(progs[i], "alias")){
				aliasCmd(inputArgs, size, copyInput);
			}
			return 1;
		}
	}
	return 0;
}

/**
 * Check if its an environment variable
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 * @return return 1
 */
int isEnv(char **inputArgs, int size, char *copyInput){
	//get "=" position
	char *pos = strstr(inputArgs[0], "=");

	//get "$" position
	char *pos2 = strstr(inputArgs[0], "$");

	if(pos){
		environment(inputArgs[0], 1, copyInput);
	}else if(pos2){
		environment(inputArgs[0], 2, copyInput);
	}else{
		return 0;
	}
	return 1;
}

/**
 * Check if its an alias
 * @param inputArgs: arguments from the command line
 * @return return 1
 */
int isAlias(char **inputArgs){
	int aliasPlace = 0;

	alias_t *al = searchAlias(inputArgs[0], &aliasPlace);

	if(al != NULL){
		execAlias(al->name, al->name, 1, 0);
		return 1;
	}else{
		return 0;
	}
}

/**
 * Process the alias
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 * @param copyInput: copy of the command line
 * @return if it's an environment variable return 1
 */
int aliasCmd(char **inputArgs, int size, char *copyInput){

	if(!strcmp(inputArgs[0], "alias")){

		//We add or show all alias with -p
		if(size >= 2){

			//get the position of the "=" or "-"
			char *pos = strstr(inputArgs[1], "=");
			char *pos2 = strstr(inputArgs[1], "-");

			//If there is "="
			if(pos || pos2){
				processAlias(inputArgs[1], copyInput);
				return 1;
				//If we ask for one alias by name
			}else if(!isChar(inputArgs[1])){
				alias_t *al = searchAlias(inputArgs[1], NULL);

				//Check if the alias exist
				if(al != NULL){
					showAlias(al);
				}else{
					printf("alias: %s: not found\n",inputArgs[1]);
				}
				return 1;
			}else{
				return 0;
			}
			//If only "alias", show all
		}else if(size == 1){
			int i;
			for (i = 0; i < countAlias; ++i) {
				showAlias(&alias[i]);
			}
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

/**
 * Search an alias in the alias array
 * @param name: alias that we search
 * @return if the alias is found return it, otherwise NULL
 */
alias_t *searchAlias(char *name, int *place ){
	int i;
	for (i = 0; i < countAlias; ++i) {

		//Compare current alias.name to to alias name
		if(!strcmp(name, alias[i].name)){

			//Return by address the value of the place where is found the alias
			if (place != NULL){
				*place = i;
			}
			return &alias[i];
		}
	}
	return NULL;
}

/**
 * Find in our chain if there is an special char
 * @param input: one arguments from the command line
 * @return 0 = false, 1 = yes
 */
int isChar(char * input){
	int i;
	for (i = 0; i < strlen(input); ++i) {
		if( input[i] == '-' || input[i] == '$' || input[i] == '/' || input[i] == '\'' || input[i] == '"' || input[i] == '<' || input[i] == '>'){
			return 1;
		}
	}
	return 0;
}

/**
 * Add/print/check the alias
 * @param input: one arguments from the command line
 * @param copyInput: copy of the command line
 */
void processAlias(char *input, char *copyInput){
	//Split the alias if there is "-"
	int countResult1;
	char **result = strsplit(input, "-", &countResult1);

	//Check if the argument after the "-" is the "p" => show all alias
	if(!strcmp(result[0], "p")){
		int i;
		for (i = 0; i < countAlias; ++i) {
			showAlias(&alias[i]);
		}

	}else {

		//Split the alias name=value and remove "alias " and keep the alias
		int countResult2;
		char **result2 = strsplit(removeString(copyInput), "=", &countResult2);
		int aliasPlace = 0;

		//If alias is !NULL we return the aliasPlace by address
		alias_t *al = searchAlias(result2[0], &aliasPlace);

		//If the alias is not in the array we increase the "aliasPlace"
		if(al == NULL){
			aliasPlace = countAlias++;
		}

		//Check is there is an space in the alias value
		if(strstr(result2[1], " ")){

			//If ' or " we can have space
			if(strstr(result2[1], "\"") || strstr(result2[1], "'")){

				//Add or replace the alias
				alias[aliasPlace].name = result2[0];
				alias[aliasPlace].value = result2[1];
			}else{
				printf("alias value is not valid. \n \t have a look to the help: help alias\n");
			}
		}else{
			//Add or replace the alias
			alias[aliasPlace].name = result2[0];
			alias[aliasPlace].value = result2[1];
		}
	}

	//Free split result
	freeToken(result, countResult1);
}

/**
 * Function which remove all before the alias name/value
 * @param input: arguments from the command line
 * @return the alias like a=b
 */
char *removeString(char* input){
	char* s = NULL;
	sscanf(input, "%*s%*[ ]%m[^\n]", &s);
	return s;
}

/**
 * Print one alias
 * @param al: one alias
 */
void showAlias(alias_t *al){
	printf("alias %s=%s\n", al->name, al->value);
}

/**
 * Execute the alias and search recursively if there is not any loop
 * @param currentName: name of the current alias we are looking for
 * @param baseName: name of the first alias we are looking for
 * @param firstCall: 1 = first call of this function, 0 = else
 */
void execAlias(char *currentName, char *baseName, int firstCall, int max){
	//If the name is the same as the original and it is not the first call of the function -> loop
	if((strcmp(currentName, baseName) == 0 && firstCall != 1)|| (max > (countAlias + 1))){//
		printf("Error: Alias loop\n");
	}else{//Otherwise do the recursion

		//Search the correspondence of the current alias
		alias_t *a = searchAlias(currentName, NULL);

		//If there is not, its finished -> execute value
		if(a == NULL){
			execAliasEnv(currentName);
			//Otherwise continue to search
		}else{
			max++;

			//We search the next name, we do not change the base name, it's not longer the first call
			execAlias(a->value, baseName, 0, max);
		}
	}
}

/**
 * Get/set environment variable or print string when we use the command echo
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 */
void echoCmd(char **inputArgs, int size){
	if(size >= 2){
		//get the position of the "$"
		char *pos = strstr(inputArgs[1], "$");

		//If there is "$"
		if(pos){
			environment(inputArgs[1], 0, "");
		}else{
			printf("%s\n", inputArgs[1]);
		}
	}
}

/**
 * Print the current working directory
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 */
void pwdCmd(char **inputArgs, int size){
	//Current working directory
	char cwd[MAX_INPUT];

	//Get current working directory
	getcwd(cwd, sizeof(cwd));

	printf("%s\n", cwd);
}

/**
 * Exit the program with a specific exit code
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 */
void exitCmd(char **inputArgs, int size){
	if(size >= 2){

		//Take the value of the exit code
		int errorCode = atoi(inputArgs[1]);
		exit(errorCode);
	}else{
		helpExit();
	}
}

/**
 * Change the current working directory
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 */
void cdCmd(char **inputArgs, int size){
	if(size >= 2){
		//Get environment variable if the second argument
		char *env = getStrEnv(inputArgs[1]);

		//If it's an environment variable
		if(env){
			//InputArgs is now the environment variable
			inputArgs[1] = env;
		}

		//If !success chdir
		if (chdir(inputArgs[1]) != 0) {
			printf("Power-Shell: cd: %s: No such file or directory\n", inputArgs[1]);
		}
		free(env);
	}
}

/**
 * Process (get/set) the environment variable or execute the variable
 * @param env: arguments from the command line
 * @param setEnv: define if we wanna set/get the environment arable
 */
void environment(char *env, int setEnv, char *copyInput){
	char split[1];
	char *args[MAX_INPUT];
	char *token;
	int count = 0;

	//if setEnv != 2 we execute the value of the variable, else variable is stored
	if(setEnv != 2){
		if(setEnv){
			//Char to split our string
			split[0] = '=';

			//Get the first token
			token = strtok(copyInput, split);
		}else{
			//Char to split our string
			split[0] = '$';

			//Get the first token
			token = strtok(env, split);
		}

		//Walk through other tokens
		while(token != NULL){
			//Copy the token to the array with strdup
			args[count] = strdup(token);
			if(!args[count]){perror("strdup error");}
			token = strtok(NULL, split);
			count++;
		}

		//If command process input
		if(count != 0){
			//Check if we want to set/get the environment variable
			if(setEnv){
				//Set environment variable
				setenv(args[0],args[1], 1);

			}else{
				char *g = getenv(args[0]);

				//Check if the environment variable is settled
				if(g != NULL){
					printf("%s\n",g);
				}else{
					printf("Error: Environment variable %s dosen't exist\n",args[0]);
				}
			}
		}
	}else{
		//Get if it's like "$a"
		char *e = getStrEnv(env);

		//Check if it's an environment variable valid
		if(e != NULL){
			execAliasEnv(e);
		}
	}
}

/**
 * Get the environment variable with input like $a
 * @param input: environment variable
 * @return environment variable in *string
 */
char *getStrEnv(char *input){
	//Allocate main array
	char *arg = malloc(MAX_INPUT * sizeof(char));

	//get the position of the "$"
	if(strstr(input, "$")){

		//Remove the "$"
		char *tmp = strtok(input, "$");

		//If only !$
		if(tmp != NULL){
			//Get the environment variable
			arg = getenv(tmp);
			if(arg){
				strcat(arg , "\0");

				//InputArgs is now the environment variable
				return arg;
			}
		}
	}
	return NULL;
}

/**
 * Execute the value of an alias or the value of the environment variable
 * @param input: value of the alias or value of environment variable
 */
void execAliasEnv(char *input){
	char *newImput = NULL;
	//Delete " or '
	if(strstr(input, "\"")){
		newImput = removeChar(input, '"');
	}else if(strstr(input, "'")){
		newImput = removeChar(input, '\'');
	}

	char **arg;
	int count = 0;
	//if there is " or ' might have more than one argument like: ls -LR
	if(newImput != NULL){

		//Split argument like; arg[0] = ls arg[1] = -LR
		arg = strsplit(newImput, " ", &count);

		if(!isProg(arg, count, arg[0]) && !isCommande(arg, count, arg[0])){
			printf("%s: command not found\n", arg[0]);
		}
	}else{
		//Split argument like; arg[0] = ls arg[1] = -LR
		arg = strsplit(input, " ", &count);

		//If more than 1 argument => error cause it's not possible
		//to have like: ls -LR not surrounded by " or '
		if(count > 1){
			printf("%s: command not found", arg[1]);
		}else{
			if(!isProg(arg, count, arg[0]) && !isCommande(arg, count, arg[0])){
				printf("%s: command not found\n", arg[0]);
			}
		}
	}
	freeToken(arg, count);
}

/**
 * Execute the command
 * @param arg: argument of the executed function
 * @param redirect: 1 if redirection to a file, 0 else
 * @param filename: name of the file to write to
 */
void execute(char **arg, int redirect, char *filename){
	pid_t  pid;
	int    status;

	//fork a child process
	if ((pid = fork()) < 0){
		perror("Child failed");
		exit(1);
	}else if (pid == 0){ //Child process
		if(redirect){

			//Create file and redirect stdout to write into it
			mode_t mode = S_IRUSR | S_IRGRP | S_IROTH;
			int fd = open(filename, O_RDWR | O_CREAT, mode);
			dup2(fd, 1); //Redirect stdout
			close(fd);
		}

		//execute the command
		if (execvp(*arg, arg) < 0){
			perror("Exec failed");
			exit(1);
		}
		exit(0);
	}
	else { //Parent
		//Wait for child completion
		while (wait(&status) != pid);
	}
}

/**
 * Remove all the specified character from an string
 * @param input: string where specified character will be removed
 * @param remove: character to remove
 * @return the string without all the specified character
 */
char *removeChar(char *input, char remove){
	int i = 0;
	int x = 0;
	char c;

	//Copy input
	char *src = strdup(input);
	if(!src){perror("strdup error");}

	//Allocate memory for the return value
	char *dst = calloc(strlen(src), sizeof(char));
	if(!dst){perror("calloc error");}

	//Walk through all elements
	while ((c = input[i++]) != '\0') {

		//Check if if not an escape char, except space
		if(c != remove){
			dst[x++] = c;
		}
	}
	//Terminate the string with '\0'
	dst[x] = '\0';

	//Free copy of input
	free(src);
	return dst;
}

/**
 * Call all the help function
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 */
void helpCmd(char **inputArgs, int size){
	//Check command
	if(!strcmp(inputArgs[1], "pwd")){
		helpPwd();
	}else if(!strcmp(inputArgs[1], "cd")){
		helpCd();
	}else if(!strcmp(inputArgs[1], "exit")){
		helpExit();
	}else if(!strcmp(inputArgs[1], "help")){
		helpHelp();
	}else if(!strcmp(inputArgs[1], "alias") ){
		helpAlias();
	}else{
		helpHelp();
	}
}

/**
 * Help commands
 */
void helpHelp(){
	printf("help: help [func]\n\tShow the help for the called function\n");
}
void helpCd(){
	printf("cd: cd [dir]\n\tChange the current working directory\n");
}
void helpExit(){
	printf("exit: exit [number]\n\tExit the program with an specific code\n");
}
void helpPwd(){
	printf("pwd: pwd\n\tShow the name of current/working directory\n");
}
void helpAlias(){
	printf("alias: alias [-p] [name[=value] ... ]\n\tSet alias or show all or one specific alias\n");
}
void helpEcho(){
	printf("echo: echo [value]\n\tPrint the value put as argument\n");
}
