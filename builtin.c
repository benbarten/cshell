#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

void cd(char *path)
{
    char *target = malloc(PATH_MAX * sizeof(char));

    if (path == NULL || *path == '\0')
    {
        target = getenv("HOME");
        if (target == NULL)
        {
            perror("Failed to get current working directory");
            return;
        }
    }
    else if (path[0] == '~')
    {
        char *home = getenv("HOME");
        if (home == NULL)
        {
            perror("Failed to get current working directory");
            return;
        }
        char *trailing = malloc(PATH_MAX * sizeof(char));
        strncpy(trailing, path + 1, strlen(path) - 1);

        strcpy(target, home);
        strcat(target, trailing);
    }
    else
    {
        target = path;
    }

    char buf[PATH_MAX];
    char *resolved_path = realpath(target, buf);

    if (chdir(resolved_path) != 0)
    {
        perror("Failed to change directory");
        return;
    }

    setenv("PWD", resolved_path, 1);
}

void pwd()
{
    char *cwd = malloc(PATH_MAX * sizeof(char));
    if (cwd == NULL)
    {
        perror("failed to allocate memory");
        return;
    }
    if (getcwd(cwd, PATH_MAX) == NULL)
    {
        perror("failed to get current working directory");
        free(cwd);
        return;
    }
    printf("%s\n", cwd);
    free(cwd);
}

void echo(char *input)
{
    size_t len = strlen(input);

    char *output = malloc(len * sizeof(char));

    if (input[0] == '"' && input[len-1] == '"')
    {
        strncpy(output, input+1, len-2);
    } else if (input[0] == '$')
    {
        strcpy(output, getenv(input+1));
    }
    else {
        strcpy(output, input);
    }

    printf("%s\n", output);
    free(output);
}