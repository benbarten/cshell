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
		exit(1);
	}
	if (getcwd(cwd, PATH_MAX) == NULL)
	{
		perror("failed to get current working directory");
		free(cwd);
		exit(1);
	}

	printf("%s\n", cwd);
	free(cwd);
	exit(0);
}

void echo(char **args, int fd_in) {
    if (args[1] == NULL) {
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(fd_in, buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read);
        }
    } else {
        for (size_t i = 1; args[i] != NULL; i++) {
            printf("%s", args[i]);
            if (args[i+1] != NULL) {
                printf(" ");
            }
        }
        printf("\n");
    }
}

static int set_env_var(const char *name, const char *value)
{
	if (name == NULL || value == NULL)
	{
		fprintf(stderr, "Error: Name and value must not be NULL\n");
		return -1;
	}

	if (setenv(name, value, 1) != 0)
	{
		fprintf(stderr, "Error setting environment variable: %s\n", strerror(errno));
		return -1;
	}

	printf("Set %s=%s\n", name, value);
	return 0;
}

static void get_env_var(const char *name)
{
	if (name == NULL)
	{
		fprintf(stderr, "Error: Name must not be NULL\n");
		return;
	}

	const char *value = getenv(name);
	if (value == NULL)
	{
		printf("Environment variable %s not found\n", name);
	}
	else
	{
		printf("%s=%s\n", name, value);
	}
}

void env(int argc, char **args)
{
	if (argc == 1)
	{
		extern char **environ;
		for (size_t i = 0; environ[i] != NULL; i++)
		{
			printf("%s\n", environ[i]);
		}
	}
	else if (argc == 2)
	{
		get_env_var(args[1]);
	}
	else if (argc == 3)
	{
		set_env_var(args[1], args[2]);
	}
	else
	{
		fprintf(stderr, "Usage: env [VARIABLE] [VALUE]\n");
	}
	exit(0);
}