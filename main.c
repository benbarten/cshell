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

void handle(char **args, int count)
{
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
        echo(args[1]);
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