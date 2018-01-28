/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		path.h
 Project:	ioc
 *****************************/

#ifndef PATH_H_
#define PATH_H_

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>

#include "split.h"

int isFile(const char* path);
int isDir(const char* path);
int isExe(const char *path);
char *getPath(char *input);
int checkPath(char *path, char *prog);

#endif /* PATH_H_ */
