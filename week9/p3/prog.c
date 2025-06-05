#include<sys/wait.h>
#include<sys/types.h>

int my_sys(const char *cm) {
    if(cm == NULL) {
        return -1;
    }
    pid_t pid = fork();
    if(pid == -1) {
        printf("error\n");
        return -1;
    } else if(pid == 0) {
        execl("/bin/sh", "sh", "-c", cm, (char *)NULL);
        printf("execerror\n");
        exit(EXIT_FAILURE);
    } else {
        int st;
        if(waitpid(pid, &st, 0) == -1) {
            return -1;
        }
        if(WIFEXITED(st)) {
            return WEXITSTATUS(st);
        } else {
            return -1;
        }
    }
}

int main() {
    // Example usage of the my_sys function
    const char *command = "ls -l";
    int result = my_sys(command);
    printf("Command executed with exit code: %d\n", result);
    return 0;
}
