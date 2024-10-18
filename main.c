#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
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
void handle(char **args, int count)
{
	int fd_in = -1, fd_out = -1;
	int saved_stdin = dup(STDIN_FILENO);
	int saved_stdout = dup(STDOUT_FILENO);

	for (int i = 0; i < count; i++)
	{
		if (strcmp(args[i], "<") == 0)
		{
			fd_in = open(args[i + 1], O_RDONLY);
			if (fd_in < 0)
			{
				perror("open");
				return;
			}
			dup2(fd_in, STDIN_FILENO);
			args[i] = NULL;
		}
		else if (strcmp(args[i], ">") == 0)
		{
			fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd_out < 0)
			{
				perror("open");
				return;
			}
			dup2(fd_out, STDOUT_FILENO);
			args[i] = NULL;
		}
		else if (strcmp(args[i], ">>") == 0)
		{
			fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd_out < 0)
			{
				perror("open");
				return;
			}
			dup2(fd_out, STDOUT_FILENO);
			args[i] = NULL;
		}
	}

	char *cmd = args[0];

	if (strncmp(cmd, PWD, 3) == 0)
	{
		pwd();
	}
	else if (strncmp(cmd, CD, 2) == 0)
	{
		cd(args[1]);
	}
	else if (strncmp(cmd, ECHO, 4) == 0)
	{
		echo(args, fd_in);
	}
	else if (strncmp(cmd, EXIT, 4) == 0)
	{
		exit(0);
	}
	else if (strncmp(cmd, ENV, 3) == 0)
	{
		env(count, args);
	}
	else
	{
		execute(args);
	}

	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	if (fd_in != -1)
		close(fd_in);
	if (fd_out != -1)
		close(fd_out);
	return;
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

		handle(args, count);
	};

	return 0;
}