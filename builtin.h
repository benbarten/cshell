#ifndef BUILTIN_H
#define BUILTIN_H

void cd(char* path);
char* pwd();
char* echo(char** input);
char** env(int argc, char **args);

#endif // BUILTIN_H