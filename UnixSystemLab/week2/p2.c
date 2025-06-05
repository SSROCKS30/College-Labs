#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Check if a filename was provided as a command-line argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];  // The filename from the command-line argument

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    // Create the file with the specified name
    int fd = creat(filename, mode);

    if (fd == -1) {
        perror("creat() error");
        exit(EXIT_FAILURE);
    }

    // Success message after file is created
    printf("File \"%s\" has been created successfully.\n", filename);

    // Close the file descriptor
    if (close(fd) == -1) {
        perror("close() error");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
