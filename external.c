#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int execute(char **args) {
    if (args[0] == NULL) {
        fprintf(stderr, "Error: No command provided\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        execvp(args[0], args);
        
        fprintf(stderr, "execvp failed for %s: %s\n", args[0], strerror(errno));
        perror("execvp error");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}