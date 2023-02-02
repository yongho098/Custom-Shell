#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "io_helpers.h"


// ===== Output helpers =====

/* Prereq: str is a NULL terminated string
 */
void display_message(char *str) {
    write(STDOUT_FILENO, str, strnlen(str, MAX_STR_LEN));
}


/* Prereq: pre_str, str are NULL terminated string
 */
void display_error(char *pre_str, char *str) {
    write(STDERR_FILENO, pre_str, strnlen(pre_str, MAX_STR_LEN));
    write(STDERR_FILENO, str, strnlen(str, MAX_STR_LEN));
    write(STDERR_FILENO, "\n", 1);
}


// ===== Input tokenizing =====

/* Prereq: in_ptr points to a character buffer of size > MAX_STR_LEN
 * Return: number of bytes read
 */
ssize_t get_input(char *in_ptr) {
    int retval = read(STDIN_FILENO, in_ptr, MAX_STR_LEN+1); // Not a sanitizer issue since in_ptr is allocated as MAX_STR_LEN+1
    int read_len = retval;
	if (retval == -1) {
	read_len = 0;
    }
    if (read_len > MAX_STR_LEN) {
        read_len = 0;
        retval = -1;
        write(STDERR_FILENO, "ERROR: input line too long\n", strlen("ERROR: input line too long\n"));
        int junk = 0;
        while((junk = getchar()) != EOF && junk != '\n');
    }
    in_ptr[read_len] = '\0';
    return retval;
}

/* Prereq: in_ptr is a string, tokens is of size >= len(in_ptr)
 * Warning: in_ptr is modified
 * Return: number of tokens.
 */
size_t tokenize_input(char *in_ptr, char **tokens) {
    char *curr_ptr = strtok (in_ptr, DELIMITERS);
    size_t token_count = 0;

    while (curr_ptr != NULL) {
       tokens[token_count] = curr_ptr;
	    token_count++;
        curr_ptr = strtok (NULL, DELIMITERS);
    }
    tokens[token_count] = NULL;
    return token_count;
}

size_t tokenize_pipe(char *in_ptr, char **tokens){
    char *curr_ptr = strtok (in_ptr, DELIMITERS_PIPE);
    size_t token_count = 0;
    if (curr_ptr == NULL){
            tokens[token_count] = "|";
            token_count++;
        }

    while (curr_ptr != NULL) {
       tokens[token_count] = curr_ptr;
	    token_count++;
        if ((curr_ptr = strtok (NULL, DELIMITERS_PIPE)) != NULL){
            tokens[token_count] = "|";
            token_count++;
        }
    }
    tokens[token_count+1] = NULL;
    return token_count;
}


size_t tokenize_pipe_2(char **inputtoken, char **outputtoken, int count){
    //
    int j;
    
    size_t token_count = 0;

    for (j = 0; j < count; j++){
        char *curr_ptr = strtok (inputtoken[j], DELIMITERS);

        while (curr_ptr != NULL) {
        outputtoken[token_count] = curr_ptr;
	    token_count++;
        curr_ptr = strtok (NULL, DELIMITERS);
    }
    }
    outputtoken[token_count+1] = NULL;
    return token_count;
}