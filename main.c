#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include "builtin.h"
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

void execute(char **args, int count, int fd_in)
{
	char *cmd = args[0];

	if (cmd == NULL)
	{
		fprintf(stderr, "Error: No command provided\n");
		return;
	}

	// EXIT and CD are both commands which transform the current process
	if (strcmp(cmd, EXIT) == 0)
	{
		exit(0);
	}
	else if (strcmp(cmd, CD) == 0)
	{
		cd(args[1]);
	}
	else
	{
		// any other process will be forked

		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork failed");
			return;
		}
		else if (pid == 0)
		{
			if (strcmp(cmd, PWD) == 0)
			{
				pwd();
			}
			else if (strcmp(cmd, ECHO) == 0)
			{
				echo(args, fd_in);
			}
			else if (strcmp(cmd, ENV) == 0)
			{
				env(count, args);
			}
			else
			{
				execvp(args[0], args);
				fprintf(stderr, "command failed %s: %s\n", args[0], strerror(errno));
			}
		}
		else
		{
			int status;
			waitpid(pid, &status, 0);
		}
	}
}

int handle(char **args, size_t argsc)
{
	// save stdin/stdout file descriptors for io redirection
	int fd_in = -1, fd_out = -1;
	int saved_stdin = dup(STDIN_FILENO);
	int saved_stdout = dup(STDOUT_FILENO);

	// parse io redirection args
	for (size_t i = 0; i < argsc; i++)
	{
		if (strcmp(args[i], "<") == 0)
		{
			fd_in = open(args[i + 1], O_RDONLY);
			if (fd_in < 0)
			{
				perror("Failed to open input file");
				return -1;
			}
			dup2(fd_in, STDIN_FILENO);
			args[i] = NULL;
		}
		else if (strcmp(args[i], ">") == 0)
		{
			fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd_out < 0)
			{
				perror("Failed to open output file");
				return -1;
			}
			dup2(fd_out, STDOUT_FILENO);
			args[i] = NULL;
		}
		else if (strcmp(args[i], ">>") == 0)
		{
			fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd_out < 0)
			{
				perror("Failed to open output file for appending");
				return -1;
			}
			dup2(fd_out, STDOUT_FILENO);
			args[i] = NULL;
		}
	}

	// execute the command
	execute(args, argsc, fd_in);

	// restore stdin/stdout file descriptors
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	if (fd_in != -1)
		close(fd_in);
	if (fd_out != -1)
		close(fd_out);

	return 0;
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

		size_t argsc = 0;
		char **args = split(input, &argsc, DELIM);

		expand_env_vars(args);

		if (argsc == 0)
		{
			continue;
		}

		handle(args, argsc);
	};

	return 0;
}