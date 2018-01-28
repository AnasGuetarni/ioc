/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		input.c
 Project:	ioc
 *****************************/

#include "input.h"

/**
 * Get the line from the user and clean it
 */
void getInput(){
	char input[MAX_INPUT];

	//Current working directory
	char cwd[MAX_INPUT];

	//Get current working directory
	getcwd(cwd, sizeof(cwd));

	strtok(cwd, "\n");

	//Print current working directory
	printf("%s>", cwd);

	//Get the chain of the command
	fgets(input, MAX_INPUT, stdin);

	char *result = cleanInput(input);
	if(result){
		decodeImput(result);
	}
}

/**
 * Check if the input is a redirection, a pipe or an program
 * @param input: arguments from the command line
 */
void decodeImput(char *input){
	//Delete the "\n"
	char *newInput = strtok(input, "\n");

	if(strstr(newInput, "<") || strstr(newInput, ">")){
		execRedirect(newInput);
	}else if(strstr(newInput, "|")){
		execPipe(newInput);
	}else{	//execute prog/command

		int count;
		char **arg = strsplit(newInput, " ", &count);
		char *copyInput = strdup(newInput);
		processInput(arg, count, copyInput);

		//Free args
		free(copyInput);
		freeToken(arg, count);
	}

	//Free input
	free(input);
}

/**
 * Call our function to execute command
 * @param inputArgs: arguments from the command line
 * @param size: arguments size
 * @param copyInput: copy of the command line
 */
void processInput(char **inputArgs, int size, char *copyInput){
	//Check if its an alias, prog or environment variable
	if(!isAlias(inputArgs) && !isProg(inputArgs, size, copyInput) && !isEnv(inputArgs, size, copyInput)){
		//check if its an external command
		if(!isCommande(inputArgs, size, copyInput)){
			printf("%s: command not found\n", inputArgs[0]);
		}
	}
}

/**
 * Execute pipe command
 * @param input: arguments from the command line
 */
void execPipe(char *input){
	int countPipe, countArg1, countArg2;
	char **pipeArgs = strsplit(input, "|", &countPipe);

	if(countPipe == 2){

		//Get argument to execute the program
		char **progArgs1 = strsplit(pipeArgs[0], " ", &countArg1);
		char **progArgs2 = strsplit(pipeArgs[1], " ", &countArg2);
		char *path1;
		char *path2;

		//Get path of the program
		if((path1 = getPath(progArgs1[0])) != NULL){
			if((path2 = getPath(progArgs2[0])) != NULL){

				//Replace the first argument
				progArgs1[0] = path1;
				progArgs2[0] = path2;

				int fd[2];
				pid_t pid1, pid2;

				if(pipe(fd)){
					perror("pipe error");
				}

				//Create the first child and run the first command
				pid1 = fork();
				if (pid1<0) {
					perror("First fork() failed!");
				}
				if (pid1==0) {

					//Set the process output to the input of the pipe
					close(1);
					dup(fd[1]);
					close(fd[0]);
					close(fd[1]);
					execvp(*progArgs1, progArgs1);
					perror("First execvp() failed");
					exit(0);
				}

				//Create the second child and run the second command
				pid2 = fork();
				if (pid2<0) {
					perror("Second fork() failed!");
				}
				if (pid2==0) {

					//Set the process input to the output of the pipe
					close(0);
					dup(fd[0]);
					close(fd[0]);
					close(fd[1]);
					execvp(*progArgs2, progArgs2);
					perror("Second execvp() failed");
					exit(0);
				}
				close(fd[0]);
				close(fd[1]);

				//Wait for the children to finish, then exit
				waitpid(pid1,NULL,0);
				waitpid(pid2,NULL,0);
			}else{
				printf("%s: command not found\n", progArgs1[0]);
			}
		}else{
			printf("%s: command not found\n", progArgs2[0]);
		}

		//Free args and pipe args
		freeToken(pipeArgs, countPipe);
		freeToken(progArgs1, countArg1);
		freeToken(progArgs2, countArg2);
	}else{
		printf("Only one | allowed\n");
	}
}

/**
 * Execute redirection to/from file
 * @param input: arguments from the command line
 */
void execRedirect(char *input){
	//If redirect command to file
	if(strstr(input, ">")){
		int countRedirect, countArg;
		char **redirect = strsplit(input, ">", &countRedirect);

		//only one redirection authorized per line
		if(countRedirect == 2){

			//Get argument to execute the program
			char **arg = strsplit(redirect[0], " ", &countArg);

			//Remove the first space in the filename
			//split be like: ps > file.txt
			//result split " file.txt"
			memmove(redirect[1], redirect[1]+1, strlen(redirect[1]));

			//It is the last argument
			char *filename = redirect[1];

			char *path;
			//Get path of the program
			if((path = getPath(arg[0])) != NULL){

				//Replace the first argument
				arg[0] = path;

				//Execute the program
				execute(arg, 1, filename);

				//Free arguments
				freeToken(arg, countArg);
			}else{
				printf("%s: command not found\n", arg[0]);
			}

		}else{
			printf("Only one > allowed\n");
		}

	}else{
		int countRedirect, countArg;
		char **redirect = strsplit(input, "<", &countRedirect);

		//Merge both argument after and before the "<"
		strcat(redirect[0], redirect[1]);

		//Only one redirection authorized per line
		if(countRedirect == 2){

			//Get argument to execute the program
			char **arg = strsplit(redirect[0], " ", &countArg);

			char *path;
			//Get path of the program
			if((path = getPath(arg[0])) != NULL){

				//Replace the first argument
				arg[0] = path;

				//Execute the program
				execute(arg, 0, NULL);

				//Free arguments
				freeToken(arg, countArg);
			}else{
				printf("%s: command not found\n", arg[0]);
			}
		}else{
			printf("Only one < is allowed\n");
		}
	}
}

/**
 * Remove double (or more) space
 * @param input: arguments from the command line
 * @return chain cleaned of extra space
 */
char *removeWhitespace(char *input){
	//Copy input
	char *src = strdup(input);
	if(!src){perror("strdup error");}

	//Allocate memory for the return value
	char *dst = calloc(strlen(src), sizeof(char));
	if(!dst){perror("calloc error");}

	int readIndex = 0, writeIndex;

	//Skips all spaces before first characters
	while(src[readIndex] == ' '){
		readIndex++;
	}

	//Walk through all elements
	for(writeIndex = 0;src[readIndex] != '\0'; readIndex++){
		if(src[readIndex]==' ' && src[readIndex-1]==' '){
			continue;
		}
		dst[writeIndex] = src[readIndex];
		writeIndex++;
	}
	//Terminate the string with '\0'
	dst[writeIndex] = '\0';

	//Free copy of input
	free(src);
	return dst;
}

/**
 * Remove escape char
 * @param inputArgs: arguments from the command line
 * @return chain cleaned of extra escape char
 */
char *removeSpecialChar(char *input){
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
		if(!isspace(c) || c == ' '){
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
 * Call function clean space/special char
 * @param inputArgs: arguments from the command line
 * @return command line cleaned
 */
char *cleanInput(char *input){
	//If different of just "enter"
	if(input[0] != '\n'){
		char *result = removeSpecialChar(input);
		char *result2 = removeWhitespace(result);
		free(result);
		return result2;
	}else{
		return NULL;
	}
}
