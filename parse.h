#ifndef PARSE_H
#define PARSE_H

char **split(char *input, size_t *argsc, char *delim);
void expand_env_vars(char **args);

#endif 