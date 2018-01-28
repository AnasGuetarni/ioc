/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		profil.c
 Project:	ioc
 *****************************/

#include "profil.h"

/**
 * Set the current directory in the console
 */
void get_path(){
	getProfilDir();

	//Get the current directory
	char *str = getenv("HOME");

	//If success chdir
	if (chdir(str) == 0) {
		getcwd(str, MAX_INPUT);
		perror("chdir error");
	}
}

/**
 * Get the current working directory of our program
 */
void getProfilDir(){
	//Current working directory
	char cwd[MAX_INPUT];

	//Get current working directory
	getcwd(cwd, sizeof(cwd));

	//Get environment variable
	getProfil(cwd);
}

/**
 * Read our profile file and get the HOME and PATH environment variable
 * @param path: the current working directory of our program
 */
void getProfil(char *path){
	FILE *file;
	char line[MAX_INPUT];
	int env = 0;

	//Profile file
	char *currentDir = strcat(path, "/profile");

	//Open file
	file = fopen(currentDir, "r+");

	//Check if file is well opened
	if(file){

		//Walk through all file
		while(fgets(line, sizeof(line), file) != NULL ){

			//Check if "#" (commentary) is in the file line
			char *position = strstr(line, "#");

			//If not we process the line
			if(!position){
				char *var = processLine(line);
				if(!strcmp(var,"PATH")){
					env++;
				}if(!strcmp(var,"HOME")){
					env++;
				}
			}
		}

		//Check if both environment variable are in the file
		if(env != 2){
			printf("Environment variable PATH or/and HOME are not in the profile file");
		}

	}else{
		perror("open file error");
	}
}

/**
 * Process each line in the profile file
 * @param line: environment variable
 * @return the name of the variable to check then is we have both PATH and HOME
 */
char *processLine(char *line){
	//Delete the "\n" for setenv
	char *newline = strtok(line, "\n");

	int count;
	char **args = strsplit(newline, "=", &count);

	//Check if there is 2 elements between the "="
	if(count == 2){
		//Set environment variable
		setenv(args[0],args[1],1);

		return args[0];
	}else{
		return NULL;
	}
}
