#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "builtin.h"
#include "external.h"
#include "parse.h"

#define PROMPT "$ "
#define DELIM " "
#define INVALID_CMD "invalid command: %s\n"

// Supported built-in POSIX commands
#define PWD "pwd"
#define CD "cd"
#define ECHO "echo"
#define EXIT "exit"
#define ENV "env"

// handle handles the args provided by the user and executes the command returning its output
char **handle(char **args, int count)
{
	char *cmd = args[0];
	char** output = NULL;

	if (strncmp(cmd, PWD, 3) == 0)
	{
		char* out = pwd();
		output = malloc(sizeof(char*));
		output[0] = out;
	}
	else if (strncmp(cmd, CD, 2) == 0)
	{
		cd(args[1]);
	}
	else if (strncmp(cmd, ECHO, 4) == 0)
	{
		char* out = echo(args);
		output = malloc(sizeof(char*));
		output[0] = out;
	}
	else if (strncmp(cmd, EXIT, 4) == 0)
	{
		exit(0);
	}
	else if (strncmp(cmd, ENV, 3) == 0)
	{
		output = env(count, args);
	}
	else
	{
		execute(args);
	}

	return output;
}

int main()
{

	while (1)
	{
		char *pwd = getenv("PWD");
		printf("%s %s", pwd, PROMPT);

		char input[100];
		fgets(input, sizeof(input), stdin);

		input[strcspn(input, "\n")] = 0; // remove the trailing new line character

		int count = 0;
		char **args = split(input, DELIM, &count);

		expand_env_vars(args);

		if (count == 0)
		{
			continue;
		}

		char **out = handle(args, count);

		if (out != NULL)
		{
			for (int i = 0; out[i] != NULL; i++)
			{
				printf("%s\n", out[i]);
			}

			free (out);
		}
	};

	return 0;
}