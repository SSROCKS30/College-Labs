#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
void err_sys(const char *message) {
perror(message);
exit(1);
}
int main(void) {
pid_t pid;
char *env_init[] = { "USER=unknown", "PATH=/tmp",
NULL };
if ((pid = fork()) < 0) {
err_sys("fork error");
} else if (pid == 0) {
if (execle("/home/cs6c70/week9/p2/echoall", "echoall", "myarg1",
"MY ARG2", (char *)0, env_init) < 0) {
err_sys("execle error");
}
}
if (waitpid(pid, NULL, 0) < 0) {
err_sys("wait error");
}
if (execlp("/home/cs6c70/week9/p2/echoall", "echoall", "only 1 arg",
(char *)0) < 0) {
err_sys("execlp error");
}
return 0;
}
