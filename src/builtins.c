#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>

#include "builtins.h"
#include "io_helpers.h"
#include "commands.h"

// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd)
{
    // printf("asdf %s", cmd);
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0)
    {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}

// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo.
 */
ssize_t bn_echo(char **tokens)
{
    ssize_t index = 1;
    if (tokens[index] != NULL)
    {
        display_message(tokens[index]);
        display_message(" ");
        index++;
    }
    while (tokens[index] != NULL)
    {
        display_message(tokens[index]);
        index++;
        if (tokens[index] != NULL)
        {
            display_message(" ");
        }
    }
    display_message("\n");

    return 0;
}

// need to make commands for all
ssize_t bn_ls(char **tokens)
{
    // --f, --rec, --d, path
    ssize_t index = 1;
    ssize_t path_location = 0;
    int rec_check = 0;
    int f_check = 0;
    int d_check = 0;

    while (tokens[index] != NULL)
    {
        if (strcmp(tokens[index], "--f") == 0)
        {
            if (tokens[index + 1] == NULL || f_check != 0)
            {
                display_error("Error, invalid format", tokens[index]);
                return -1;
            }
            else
            {
                f_check = index + 1;
                index += 2;
            }
        }
        else if (strcmp(tokens[index], "--rec") == 0)
        {
            if (tokens[index + 1] == NULL || path_location != 0)
            {
                display_error("Error, invalid format", tokens[index]);
                return -1;
            }
            else
            {
                rec_check = index + 1;
                index += 2;
            }
        }
        else if (strcmp(tokens[index], "--d") == 0)
        {
            // depth path
            if (tokens[index + 1] == NULL || d_check != 0)
            {
                display_error("Error, invalid format", tokens[index]);
                return -1;
            }
            else
            {
                d_check = index + 1;
                index += 2;
            }
        }
        else
        {
            // path path
            if (path_location != 0 || rec_check != 0)
            {
                display_error("Error, invalid format", tokens[index]);
                return -1;
            }
            else
            {
                path_location = index;
                index++;
            }
        }
    }
    // check which path to go down
    if ((rec_check == 0 && d_check == 0) || (rec_check != 0 && d_check != 0))
    {
        if (rec_check == 0 && d_check == 0)
        {
            if (f_check == 0)
            {
                // option 1: only path, need to account for no arguement
                if (path_location == 0)
                {
                    // no arguement
                    char *base = ".";
                    return lstest(base);
                }
                else
                {
                    return lstest(tokens[path_location]);
                }
            }
            else
            {
                // option 2: path and f search
                return lspathsearch(tokens[path_location], tokens[f_check]);
            }
        }
        else
        {
            // ecuresion, ignore path
            if (f_check == 0)
            {
                // option 3: recursion, no search
                return lsrecursion(tokens[rec_check], tokens[d_check]);
            }
            else
            {
                // option 4: recursion and search
                return lsrecursionsearch(tokens[rec_check], tokens[d_check], tokens[f_check]);
            }
        }
    }
    else
    {
        display_error("invalid format", tokens[rec_check]);
        return -1;
    }

    return 0;
}

ssize_t bn_cd(char **tokens)
{
    // scenario for different amount of arguements, either 0 or 1, any more is error
    if (tokens[2] != NULL)
    {
        // error, more than 1 arguement
        display_error("ERROR: Invalid format ", tokens[0]);
        return -1;
    }
    else if (tokens[1] != NULL)
    {
        // 1 arguement, need to check for multiple dots, only if multiple dots
        return cdtest(tokens[1]);
    }
    else
    {
        // 0 arguements, return to root
        char *roots = "..";
        return cdtest(roots);
    }
}

ssize_t bn_cat(char **tokens)
{
    if (tokens[1] == NULL)
    {
        return catpipe();
        // display_error("ERROR: No input source provided", tokens[0]);
        // return -1;
    }
    else if (tokens[2] != NULL)
    {
        display_error("ERROR: Wrong format", tokens[0]);
        return -1;
    }
    else
    {
        // single path
        return cattest(tokens[1]);
    }
}

int bn_catpipe2()
{
    return catpipe();
}

ssize_t bn_wc(char **tokens)
{
    // word count
    if (tokens[1] == NULL)
    {
        display_error("ERROR: No input source provided", tokens[0]);
        return -1;
    }
    else if (tokens[2] != NULL)
    {
        display_error("ERROR: Wrong format", tokens[0]);
        return -1;
    }
    else
    {
        // single path
        return wctest(tokens[1]);
    }
}

int piper(char **tokens, int token_count, int pipecountnum)
{
    char *parent_arr[MAX_STR_LEN] = {NULL};
    int newstart = 0;
    char *child_arr[MAX_STR_LEN] = {NULL};

    int fd[2];
    // parent
    newstart = piper3(tokens, parent_arr, token_count);
    // child
    piper5(tokens, child_arr, token_count, newstart);
        if (pipe(fd) == -1)
        {
            return 1;
        }

        int pid1 = fork();
        if (pid1 < 0)
        {
            return 2;
        }

        if (pid1 == 0)
        {
            
            // child process 1, separate for each posiblilty
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            // branch off here using if and checking parent
            int comparers;
            if ((comparers = strcmp(parent_arr[0], "echo")) == 0){
                bn_echo(parent_arr);
                return 0;
            }
            else if ((comparers = strcmp(parent_arr[0], "ls")) == 0){
                if (bn_ls(parent_arr) == -1){
                    display_error("ERROR: Builtin failed: ", parent_arr[0]);
                }
                return 0;
            }
            else if ((comparers = strcmp(parent_arr[0], "cd")) == 0){
                // do nothing
                return 0;
            }
            else if ((comparers = strcmp(parent_arr[0], "cat")) == 0){
                 if (parent_arr[1] == NULL){
                    catpipe();
                }
                else{
                    if (bn_cat(parent_arr) == -1){
                        display_error("ERROR: Builtin failed: ", parent_arr[0]);
                    }
                }
                return 0;
            }
            else if ((comparers = strcmp(parent_arr[0], "wc")) == 0){
                if (parent_arr[1] == NULL){
                    wcpipe();
                }
                else{
                    if (bn_wc(parent_arr) == -1){
                        display_error("ERROR: Builtin failed: ", parent_arr[0]);
                    }
                }
                return 0;
            }
            else
				{
                char *checkstr;
				const char ch = '=';
				checkstr = strchr(parent_arr[0], ch);
				if (checkstr != NULL){
                    return 0;
                    
                }
                    else{
					display_error("ERROR: Unrecognized command: ", parent_arr[0]);
                    }
                    return 0;
				}
            
        }

        int pid2 = fork();
        if (pid2 < 0)
        {
            return 3;
        }

        if (pid2 == 0)
        {
            // child process 2, separate for each posiblilty
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            int comparers;
            if ((comparers = strcmp(child_arr[0], "echo")) == 0){
                bn_echo(child_arr);
                return 0;
            }
            else if ((comparers = strcmp(child_arr[0], "ls")) == 0){
                if (bn_ls(child_arr) == -1){
                    display_error("ERROR: Builtin failed: ", child_arr[0]);
                }
                return 0;
            }
            else if ((comparers = strcmp(child_arr[0], "cd")) == 0){
                return 0;
            }
            else if ((comparers = strcmp(child_arr[0], "cat")) == 0){
                // check if theres another followup
                if (child_arr[1] == NULL){
                    catpipe();
                }
                else{
                    if (bn_cat(child_arr) == -1){
                        display_error("ERROR: Builtin failed: ", child_arr[0]);
                    }
                }
                return 0;
            }
            else if ((comparers = strcmp(child_arr[0], "wc")) == 0){
                if (child_arr[1] == NULL){
                    wcpipe();
                }
                else{
                    if (bn_wc(child_arr) == -1){
                        display_error("ERROR: Builtin failed: ", child_arr[0]);
                    }
                }
                return 0;
            }
            else
				{
					display_error("ERROR: Unrecognized command: ", child_arr[0]);
                    return 0;
				}
        }

        close(fd[0]);
        close(fd[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

    return 1;
}

int piper3(char **tokens, char **new_output,  int token_count)
{
    // need to deal with variables, just reorganize
    // sort first and second? array of array
    int j;
    int comparer;
    for (j = 0; j < token_count; j++)
    {
        if ((comparer = strcmp(tokens[j], "|")) != 0){
            new_output[j] = tokens[j];
        }
        else{
            break;
        }
     }   
    return j;
}

int piper5(char **tokens, char **new_output, int token_count, int start){
    // same as piper3 but for the back half-child process
    int j;
    int starter = 0;
    int comparer;
    for (j = start+1; j < token_count; j++)
    {
        if ((comparer = strcmp(tokens[j], "|")) != 0){
            new_output[starter] = tokens[j];
            starter++;
            
        }
        else{
            break;
        }
     }   
    return j;
}

int piper4(char **tokens, int token_count)
{
    // debugger, shows outputs
    int j;
    for (j = 0; j < token_count; j++)
    {
        display_message("item: ");
        display_message(tokens[j]);
        display_message(" ");
    }
    display_message("\n");
    return 0;
}

int pipecounter(char **tokens, int token_count)
{
    int counter = 0;
    int comparer;
    int j;
    for (j = 0; j < token_count; j++)
    {
        if ((comparer = strcmp(tokens[j], "|")) == 0)
        {
            counter++;
        }
    }
    return counter;
}
