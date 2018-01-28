/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		path.c
 Project:	ioc
 *****************************/

#include "path.h"

/**
 * Check if specified path is a file
 * @param path: path to check
 * @return 1 if true, 0 else
 */
int isFile(const char* path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISREG(buf.st_mode);
}

/**
 * Check if specified path is a directory
 * @param path: path to check
 * @return 1 if true, 0 else
 */
int isDir(const char* path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

/**
 * Check if specified path is an executable
 * @param path: path to check
 * @return 1 if true, 0 else
 */
int isExe(const char *path){
	//Check if executable
	if (access(path, F_OK|X_OK) == 0){
		return 1;
	}else{
		return 0;
	}
}

/**
 * Check if the executable of the specified path exist
 * @param input: arguments from the command line
 * @return the path where the executable is found, NULL if not
 */
char *getPath(char *input){
	//User put path
	if(strstr(input, "/")){

		//Check if it's an exe
		if(isExe(input)){
			return input;
		}else{
			return NULL;
		}
	}else{
		//Get PATH environment variable
		char *envPath = getenv("PATH");

		int i, count;
		char **path = strsplit(envPath, ":", &count);

		//Walk throw all the path
		for (i = 0; i < count; ++i) {

			if(checkPath(path[i], input)){

				//Copy the path and merge the path and the prog
				char *pathCopy = strdup(path[i]);
				strcat(pathCopy, "/");
				strcat(pathCopy, input);

				freeToken(path, count);

				//Check if exe
				if(isExe(pathCopy)){
					return pathCopy;
				}else{
					return NULL;
				}
			}
		}
	}
	return NULL;
}

/**
 * Walks through the path and search the program
 * @param path: path to search in
 * @param prog: program to find
 * @return 1 if program is found, 0 else
 */
int checkPath(char *path, char *prog){
	//default struct created by DIR
	struct dirent *dir_struct;
	DIR* d;

	if ((d = opendir(path)) != NULL){
		while ((dir_struct = readdir(d)) != NULL) {

			//If the prog is in this path
			if(!strcmp(prog, (char*)dir_struct->d_name)){
				closedir (d);
				return 1;
			}
		}
	}
	closedir (d);
	return 0;
}
