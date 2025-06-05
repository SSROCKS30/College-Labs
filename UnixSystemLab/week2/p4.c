#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#define READ_SIZE 20

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *file_path = argv[1];
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) { perror("Error opening file"); return 1; }

    char buffer[READ_SIZE + 1] = {0};

    printf("a) First 20 characters:\n");
    read(fd, buffer, READ_SIZE);
    printf("%s\n", buffer);

    printf("\nb) 20 characters from 10th byte:\n");
    lseek(fd, 10, SEEK_SET);
    read(fd, buffer, READ_SIZE);
    printf("%s\n", buffer);

    printf("\nc) 20 characters after skipping 10 bytes from current position:\n");
    lseek(fd, 10, SEEK_CUR);
    read(fd, buffer, READ_SIZE);
    printf("%s\n", buffer);

    struct stat file_stat;
    stat(file_path, &file_stat);
    printf("\nd) File size: %ld bytes\n", file_stat.st_size);

    close(fd);
    return 0;
}
