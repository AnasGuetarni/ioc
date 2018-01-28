/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		main.c
 Project:	ioc
 *****************************/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
    //Initialization of profile
    get_path();

    //Initialization of all functions
    initProg();

    // Run command loop.
    while(1){
      getInput();
    }

    return EXIT_SUCCESS;
}
