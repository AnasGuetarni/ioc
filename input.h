/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		input.h
 Project:	ioc
 *****************************/

#ifndef INPUT_H_
#define INPUT_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <fcntl.h>

#include "split.h"
#include "path.h"
#include "functions.h"

//Functions who get commands and work with the pipe and inputs
void getInput();
void decodeImput(char *input);
void processInput(char *inputArgs[MAX_INPUT], int size, char *copyInput);
char *cleanInput(char *input);
char *removeWhitespace(char *input);
char *removeSpecialChar(char *input);
char **strsplit(const char* str, const char* delim, int* numtokens);
void execute(char **arg, int redirect, char *filename);
void execRedirect(char *input);
void execPipe(char *input);

#endif /* INPUT_H_ */
