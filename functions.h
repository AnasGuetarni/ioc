/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		functions.h
 Project:	ioc
 *****************************/

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>

#include "path.h"
#include "split.h"

// 6 progs and 64 alias maximum
#define NB_PROGS 6
#define MAX_ALIAS 64


//Structure used for alias
typedef struct Alias {
    char* name;
    char* value;
} alias_t;


void initProg();
void environment(char *env, int setEnv, char *copyInput);
int isAlias(char *inputArgs[MAX_INPUT]);
int isProg(char *inputArgs[MAX_INPUT], int size, char *copyInput);
int isEnv(char **inputArgs, int size, char *copyInput);
int isCommande(char **inputArgs, int size, char *copyInput);
void execute(char **arg, int redirect, char *filename);

//Differents commands
void echoCmd(char *inputArgs[MAX_INPUT], int size);
void exitCmd(char *inputArgs[MAX_INPUT], int size);
void cdCmd(char *inputArgs[MAX_INPUT], int size);
void pwdCmd(char *inputArgs[MAX_INPUT], int size);
void helpCmd(char *inputArgs[MAX_INPUT], int size);
int aliasCmd(char *inputArgs[MAX_INPUT], int size, char *copyInput);
char *getStrEnv(char *input);
char *removeChar(char *input, char remove);
void execAliasEnv(char *input);

//Help commands
void helpHelp();
void helpCd();
void helpExit();
void helpPwd();
void helpAlias();
void helpEcho();

//Alias commands
void showAlias(alias_t *al);
void processAlias(char *input, char *copyInput);
int isChar(char * input);
alias_t *searchAlias(char *name, int *place );
char *removeString(char* input);
void execAlias(char *currentName, char *baseName, int firstCall, int max);

#endif /* COMMAND_H_ */
