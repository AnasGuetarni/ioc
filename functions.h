//
// Created by Anas Guetarni on 20/11/2017.
//

#ifndef FUNCTIONS
#define FUNCTIONS

int ioc_cd(char **args);
int ioc_help(char **args);
int ioc_exit(char **args);
int ioc_pwd(char **args);
int ioc_alias(char **args);

int ioc_launch(char **args);
int ioc_execute(char **args);

char *ioc_read_line();
char **ioc_split_line(char *line);

void ioc_loop();

#endif FUNCTIONS
