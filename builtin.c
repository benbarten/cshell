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

char *pwd()
{
	char *cwd = malloc(PATH_MAX * sizeof(char));
	if (cwd == NULL)
	{
		perror("failed to allocate memory");
		return "";
	}
	if (getcwd(cwd, PATH_MAX) == NULL)
	{
		perror("failed to get current working directory");
		free(cwd);
		return "";
	}
	return cwd;
}

char *echo(char **input)
{
	int size = 0;
	for (int i = 1; input[i] != NULL; i++)
	{
		size += strlen(input[i]);
	}

	char *output = malloc(size * sizeof(char*));

	for (int i = 1; input[i] != NULL; i++)
	{
		// TODO handle quotes across multiple arguments

		int len = strlen(input[i]);
		if (len == 0)
		{
			continue;
		}

		char* out;

		if (input[i][0] == '"' && input[i][len-1] == '"')
		{
			strncpy(out, input[i] + 1, len - 2);
		}
		else
		{
			out = input[i];
		}
		
		if (output[0] != '\0')
		{
			strcat(output, " ");
		}
		strcat(output, out);
	}

	return output;
}

int set_env_var(const char *name, const char *value)
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

char *get_env_var(const char *name)
{
	if (name == NULL)
	{
		fprintf(stderr, "Error: Name must not be NULL\n");
		return NULL;
	}

	char *value = getenv(name);
	if (value == NULL)
	{
		printf("Environment variable %s not found\n", name);
	}
	else
	{
		printf("%s=%s\n", name, value);
	}

	return value;
}

char **env(int argc, char **args)
{
	char** output;
	if (argc == 1)
	{
		// List all environment variables
        extern char** environ;
        int env_count = 0;
        
        while (environ[env_count] != NULL) {
            env_count++;
        }
        
        output = malloc((env_count + 1) * sizeof(char*));
        if (output == NULL) {
            perror("Failed to allocate memory for environment variables");
            return NULL;
        }
        
        for (int i = 0; i < env_count; i++) {
            output[i] = strdup(environ[i]);
            if (output[i] == NULL) {
                perror("Failed to duplicate environment variable");
                for (int j = 0; j < i; j++) {
                    free(output[j]);
                }
                free(output);
                return NULL;
            }
        }
        output[env_count] = NULL;
	}
	else if (argc == 2)
	{
		// return the value of the specified environment variable
		char* env_var = get_env_var(args[1]);
		output = malloc(sizeof(char*));
		output[0] = env_var;
	}
	else if (argc == 3)
	{
		// set the value of the specified environment variable
		set_env_var(args[1], args[2]);
	}
	else
	{
		fprintf(stderr, "Usage: env [VARIABLE] [VALUE]\n");
	}

	return output;
}