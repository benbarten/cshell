#ifndef BUILTIN_H
#define BUILTIN_H

void cd(char* path);
void pwd();
void echo(char** input, int fd);
void env(int argc, char **args);

#endif // BUILTIN_H