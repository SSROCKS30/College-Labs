// Consider the last 100 bytes as a region. Write a C program to check whether the region is locked or not. If the region is locked, print pid of the process which has locked. If the region is not locked, lock the region with an exclusive lock, read the last 50 bytes and unlock the region.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd;
    struct flock lock;
    off_t filesize;
    char buffer[51];

    if (argc != 2)
        return 1;

    fd = open(argv[1], O_RDONLY);
    filesize = lseek(fd, 0, SEEK_END);

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_END;
    lock.l_start = -100;
    lock.l_len = 100;
    lock.l_pid = 0;

    if (fcntl(fd, F_GETLK, &lock) == -1)
        return 1;

    if (lock.l_type != F_UNLCK) {
        printf("Region is locked by process with PID: %d\n", lock.l_pid);
    } else {
        lock.l_type = F_WRLCK;
        fcntl(fd, F_SETLK, &lock);

        lseek(fd, -50, SEEK_END);
        read(fd, buffer, 50);
        buffer[50] = '\0';
        printf("Last 50 bytes: %s\n", buffer);

        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
    }

    close(fd);
    return 0;
}
