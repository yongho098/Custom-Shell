#ifndef __COMMANDS_H
#define __COMMANDS_H

int lstest(char *path);
int cdtest(char *path);
int lspathsearch(char *path, char *search);
int lsrecursion(char *path, char *depth);
int lsrecursionsearch(char *path, char *depth, char *search);
int cattest(char *file);
int wctest(char *file);
int catpipe();
int wcpipe();

#endif