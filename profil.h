/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		profil.h
 Project:	ioc
 *****************************/

#ifndef PROFIL_H_
#define PROFIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "split.h"

#define MAX_INPUT 1024

void getProfil(char *path);
char *processLine(char *line);
void getProfilDir();
void get_path();

#endif /* PROFIL_H_ */
