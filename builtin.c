#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define MAX_ENV_LENGTH 1024

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

int set_env_var(const char *name, const char *value) {
    if (name == NULL || value == NULL) {
        fprintf(stderr, "Error: Name and value must not be NULL\n");
        return -1;
    }

    if (setenv(name, value, 1) != 0) {
        fprintf(stderr, "Error setting environment variable: %s\n", strerror(errno));
        return -1;
    }

    printf("Set %s=%s\n", name, value);
    return 0;
}

char *get_env_var(const char *name) {
    if (name == NULL) {
        fprintf(stderr, "Error: Name must not be NULL\n");
        return NULL;
    }

    char *value = getenv(name);
    if (value == NULL) {
        printf("Environment variable %s not found\n", name);
    } else {
        printf("%s=%s\n", name, value);
    }

    return value;
}

void display_env_vars() {
    extern char **environ;
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}

void env(int argc, char **args) {
    if (argc == 1) {
        display_env_vars();
    } else if (argc == 2) {
        get_env_var(args[1]);
    } else if (argc == 3) {
        set_env_var(args[1], args[2]);
    } else {
        fprintf(stderr, "Usage: env [VARIABLE] [VALUE]\n");
    }
}