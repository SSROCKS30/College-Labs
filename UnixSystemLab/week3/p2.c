#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(){
        int fd1 = 0, fd2 = 0;
        char buf[10] = "abcdef";

        fd1 = open("t12.txt", O_RDWR);
        fd2 = dup(fd1);

        printf("%d %d\n", fd1, fd2);
        write(fd1, buf, 6);

        close(fd1);
        close(fd2);

        return 0;
}
