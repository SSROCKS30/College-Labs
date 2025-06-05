// Write a C program using sigaction system call which calls a signal handler on SIGINT signal and then reset the default action of the SIGINT signal.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int cnt = 0;
void handler(int signum) {
    if(cnt == 0) {
        printf("SIGNIT received. custom handler executed.\n");
        cnt++;
    }
    else {
        exit(0);
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);

    while (1) {
        printf("Running... Press Ctrl+C\n");
        sleep(1);
    }

    return 0;
}
