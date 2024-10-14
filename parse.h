#ifndef PARSE_H
#define PARSE_H

char **split(char *input, char *delim, int *count);
void expand_env_vars(char **args);

#endif 