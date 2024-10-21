#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "builtin.h"

#define MAX_ENV_LENGTH 1024

char **split(char *input, char *delim, int *count)
{
    // strtok modifies the string, so we need to duplicate the pointer here in order to free it up later on
    char *tmp = strdup(input);
    char *token = strtok(tmp, delim);
    while (token != NULL)
    {
        *count = *count + 1;
        token = strtok(NULL, delim);
    }
    free(tmp);

    char **tokens = malloc((*count + 1) * sizeof(char *));
    if (tokens == NULL)
    {
        return NULL;
    }

    // split the input string
    token = strtok(input, delim);
    int i = 0;

    while (token != NULL && i < *count)
    {
        tokens[i] = strdup(token);
        token = strtok(NULL, delim);
        i++;
    }
    tokens[i] = NULL;

    return tokens;
}

void expand_env_vars(char **args)
{
    for (int i = 0; args[i] != NULL; i++)
    {
        char *arg = args[i];
        char expanded[MAX_ENV_LENGTH] = "";
        int exp_index = 0;
        int arg_len = strlen(arg);

        for (int j = 0; j < arg_len; j++)
        {
            if (arg[j] == '$' && (j + 1 < arg_len) && (isalpha(arg[j + 1]) || arg[j + 1] == '_'))
            {
                char env_name[MAX_ENV_LENGTH] = "";
                int env_index = 0;
                j++;

                while (j < arg_len && (isalnum(arg[j]) || arg[j] == '_'))
                {
                    env_name[env_index++] = arg[j++];
                }
                env_name[env_index] = '\0';
                j--;

                char *env_value = getenv(env_name);
                if (env_value != NULL)
                {
                    strcat(expanded, env_value);
                    exp_index += strlen(env_value);
                }
                else
                {
                    // If the env var doesn't exist, leave it as is
                    expanded[exp_index++] = '$';
                    strcat(expanded, env_name);
                    exp_index += strlen(env_name);
                }
            }
            else
            {
                expanded[exp_index++] = arg[j];
            }
        }
        expanded[exp_index] = '\0';

        free(args[i]);
        args[i] = strdup(expanded);
    }
}
