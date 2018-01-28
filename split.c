/*****************************
 Author:	Anas Guetarni & Melvin Gay
 Date:		28.01.2018
 Name:		split.c
 Project:	ioc
 *****************************/

#include "split.h"

/**
 * Split a string into multiple tokens using delimiter
 * @param str: string to split
 * @param delim: delimiter used for split
 * @param numtokens: number of substrings (tokens) created
 * @return string array containing all the substrings
 */
char **strsplit(const char* str, const char* delim, int* numtokens){
	char *s = strdup(str);
	int tokens_alloc = 1;
	int tokens_used = 0;
	char **tokens = calloc(tokens_alloc, sizeof(char*));
	char *token, *strtok_ctx;
	for (token = strtok_r(s, delim, &strtok_ctx);
			token != NULL;
			token = strtok_r(NULL, delim, &strtok_ctx)) {
				
		// check if we need to allocate more space for tokens
		if (tokens_used == tokens_alloc) {
			tokens_alloc *= 2;
			tokens = realloc(tokens, tokens_alloc * sizeof(char*));
		}
		tokens[tokens_used++] = strdup(token);
	}
	if (tokens_used == 0) {
		free(tokens);
		tokens = NULL;
	} else {
		tokens = realloc(tokens, tokens_used * sizeof(char*));
	}
	*numtokens = tokens_used;
	free(s);
	return tokens;
}

/**
 * Free all the tokens
 * @param input: string array to free
 * @param size:	size of the array
 */
void freeToken(char **input, int size){
	int i;
	for(i = 0; i < size; i++){
		free(input[i]);
	}
	free(input);
}
