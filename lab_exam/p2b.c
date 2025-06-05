// Write a C program to simulate system function.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int my_system(const char *command) {
    if (command == NULL)
        return 1;

    pid_t pid = fork();

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(127);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
}

int main() {
    my_system("ls -l");
    return 0;
}
