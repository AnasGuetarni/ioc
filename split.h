/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		split.h
 Project:	ioc
 *****************************/

#ifndef SPLIT_H_
#define SPLIT_H_

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


char **strsplit(const char* str, const char* delim, int* numtokens);
void freeToken(char **input, int size);

#endif /* SPLIT_H_ */
