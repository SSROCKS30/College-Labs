#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("\nUsage: %s <pathname>\n", argv[0]);
        exit(1);
    }

    if (access(argv[1], R_OK) < 0) {
        perror("\nAccess error");
    } else {
        printf("\nRead access OK\n");
    }

    int fd;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("\nOpen error");
    } else {
        printf("\nOpen for reading OK\n");
        close(fd);
    }

    exit(0);
}
