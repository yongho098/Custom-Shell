#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <poll.h>

#include "builtins.h"
#include "io_helpers.h"

int lsrecursionbase(char *path, int depth, int current);
int lsrecursionsearchbase(char *path, int depth, int current, char *searcher);

int lstest(char *path)
{
    // only path
    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(path)) == NULL)
    {
        display_error("ERROR: Invalid path", path);
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        display_message(dirp->d_name);
        display_message("\n");
    }
    closedir(dp);
    return 1;
}

int lspathsearch(char *path, char *search)
{
    // path and search
    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(path)) == NULL)
    {
        display_error("ERROR: Invalid path", path);

        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        char *comparer = strstr(dirp->d_name, search);
        if (comparer)
        {
            display_message(dirp->d_name);
            display_message("\n");
        }
    }
    closedir(dp);
    return 1;
}

// recursion function

int lsrecursion(char *path, char *depth)
{
    int new_depth = *depth - '0';
    return lsrecursionbase(path, new_depth, 1);
}

int lsrecursionsearch(char *path, char *depth, char *search)
{
    // recursionsearch
    int new_depth = *depth - '0';
    return lsrecursionsearchbase(path, new_depth, 1, search);
}

int lsrecursionbase(char *path, int depth, int current)
{
    // only path
    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(path)) == NULL)
    {
        display_error("ERROR: Invalid path", path);
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        if (dirp->d_type == DT_DIR)
        {
            if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
            {
                display_message(dirp->d_name);
                display_message("\n");
            }
            else
            {
                display_message(dirp->d_name);
                display_message("\n");
                if (current < depth)
                {
                    char new_path[1024];
                    snprintf(new_path, sizeof(new_path), "%s/%s", path, dirp->d_name);
                    lsrecursionbase(new_path, depth, current + 1);
                }
            }
        }
        else
        {
            display_message(dirp->d_name);
            display_message("\n");
        }
    }
    closedir(dp);
    return 1;
}

int lsrecursionsearchbase(char *path, int depth, int current, char *searcher)
{
    DIR *dp;
    struct dirent *dirp;

    if ((dp = opendir(path)) == NULL)
    {
        display_error("ERROR: Invalid path", path);
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        if (dirp->d_type == DT_DIR)
        {
            if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
            {
                // nothing
            }
            else
            {
                char *comparer = strstr(dirp->d_name, searcher);
                if (comparer)
                {
                    display_message(dirp->d_name);
                    display_message("\n");
                    if (current < depth)
                    {
                        char new_path[1024];
                        snprintf(new_path, sizeof(new_path), "%s/%s", path, dirp->d_name);
                        lsrecursionbase(new_path, depth, current + 1);
                    }
                }
                else
                {
                    if (current < depth)
                    {
                        char new_path[1024];
                        snprintf(new_path, sizeof(new_path), "%s/%s", path, dirp->d_name);
                        lsrecursionbase(new_path, depth, current + 1);
                    }
                }
            }
        }
        else
        {
            char *comparer = strstr(dirp->d_name, searcher);
            if (comparer)
            {
                display_message(dirp->d_name);
                display_message("\n");
            }
        }
    }
    closedir(dp);
    return 1;
}

int cdtest(char *path)
{
    size_t length = strlen(path);
    size_t i = 0;
    size_t counter = 0;
    size_t dot_count = 0;

    for (i = 0; i < length; i++)
    {
        if (path[i] == '.')
        {
            dot_count++;
        }
        else
        {
            counter++;
        }
    }

    if (counter > 0)
    {
        // non dot path
        if (chdir(path) == 0)
        {
            return 1;
        }
        else
        {
            display_error("ERROR: Invalid path", path);
            return -1;
        }
    }
    else
    {
        // dot path
        if (dot_count <= 2)
        {
            // .. path
            if (chdir(path) == 0)
            {
                return 1;
            }
            else
            {
                display_error("ERROR: Invalid path", path);
                return -1;
            }
        }
        else
        {
            // pure dot
            char catt[4] = "/..";
            char newout[100] = "..";
            int i_two = 2;
            for (i_two = 2; i_two < length; i_two++)
            {
                strcat(newout, catt);
            }
            if (chdir(newout) == 0)
            {
                return 1;
            }
            else
            {
                display_error("ERROR: Invalid path", newout);
                return -1;
            }
        }
    }
}

int cattest(char *file)
{
    //
    FILE *fptr;
    char chunk[4000];
    fptr = fopen(file, "r");
    if (fptr == NULL)
    {
        display_error("ERROR: Cannot open file: ", file);
        return -1;
    }

    while (fgets(chunk, sizeof(chunk), fptr) != NULL)
    {
        display_message(chunk);
    }
    display_message("\n");

    fclose(fptr);
    return 1;
}

int catpipe(){
    FILE *fptr = NULL;
    struct pollfd fds;
    fds.fd = 0;     
    fds.events = POLLIN; 
    int ret = poll(&fds, 1, 10); 
    if (ret == 0) {
        display_message("There's nothing to read from the fds\n");
    } else {
    fptr = stdin;
    char c = fgetc(fptr);
    char dest[2] = "\0";
    while (c != EOF) {
        dest[0] = c;
      display_message(dest);
      c = fgetc(fptr);
    }
  }
  return 0;
}

int wctest(char *file)
{
    // word count, char count, newline count

    FILE *fileopen;

    char ch[2] = " ";
    char previous[2] = " ";
    int characters, words, lines;

    // open file
    fileopen = fopen(file, "r");

    // check if file opened
    if (fileopen == NULL)
    {
        display_error("ERROR: Cannot open file", file);
        return -1;
    }

    // counter
    characters = words = lines = 0;
    while ((ch[0] = fgetc(fileopen)) != EOF)
    {
        characters++;

        // check if newline
        if (ch[0] == '\n' || ch[0] == '\0')
        {
            lines++;
        }

        // check if word against previous
        if (ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\0')
        {
            if (previous[0] == ' ' || previous[0] == '\t' || previous[0] == '\n' || previous[0] == '\0')
            {
                // do nothig
            }
            else
            {
                words++;
            }
        }
        strcpy(previous, ch);
    }

    // increment for last word if not newline
    if (characters > 0)
    {
        if (previous[0] == ' ' || previous[0] == '\t' || previous[0] == '\0' || previous[0] == '\n')
        {

        }
        else
        {

            words++;
            lines++;
        }
    }

    // convert int to str
    char characterout[characters + 2];
    char wordsout[words + 2];
    char lineout[lines + 2];

    sprintf(characterout, "%d", characters);
    sprintf(wordsout, "%d", words);
    sprintf(lineout, "%d", lines);

    // output
    display_message("word count ");
    display_message(wordsout);
    display_message("\n");
    display_message("character count ");
    display_message(characterout);
    display_message("\n");
    display_message("newline count ");
    display_message(lineout);
    display_message("\n");

    // close file
    fclose(fileopen);
    return 1;
}

int wcpipe()
{
    FILE *fptr = NULL;
    char ch[2] = " ";
    char previous[2] = " ";
    int characters, words, lines;
    struct pollfd fds;

    fds.fd = 0;
    fds.events = POLLIN;
    int ret = poll(&fds, 1, 10); 
    if (ret == 0)
    {
        display_message("There's nothing to read from the fds\n");
    }
    else
    {
        fptr = stdin;
        characters = words = lines = 0;

        while ((ch[0] = fgetc(fptr)) != EOF)
        {
            characters++;

            // check if newline
            if (ch[0] == '\n' || ch[0] == '\0')
            {
                lines++;
            }

            // check if word against previous
            if (ch[0] == ' ' || ch[0] == '\t' || ch[0] == '\n' || ch[0] == '\0')
            {
                if (previous[0] == ' ' || previous[0] == '\t' || previous[0] == '\n' || previous[0] == '\0')
                {
                    // do nothig
                }
                else
                {
                    words++;
                }
            }
            strcpy(previous, ch);
        }

        // increment for last word if not newline
        if (characters > 0)
        {
            if (previous[0] == ' ' || previous[0] == '\t' || previous[0] == '\0' || previous[0] == '\n')
            {
                // lines++;
            }
            else
            {

                words++;
                lines++;
            }
        }
        // convert int to str
        characters--;
        char characterout[characters + 2];
        char wordsout[words + 2];
        char lineout[lines + 2];

        sprintf(characterout, "%d", characters);
        sprintf(wordsout, "%d", words);
        sprintf(lineout, "%d", lines);

        // output
        display_message("word count ");
        display_message(wordsout);
        display_message("\n");
        display_message("character count ");
        display_message(characterout);
        display_message("\n");
        display_message("newline count ");
        display_message(lineout);
        display_message("\n");
    }
    return 0;
}