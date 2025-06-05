#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1024  // Define buffer size for reading

int main() {
    char buffer[BUFFER_SIZE];  // Buffer to store input
    ssize_t bytesRead;

    // Read input from standard input (keyboard)
    while ((bytesRead = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        // Write output to standard output (screen)
        if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead) {
            perror("Error writing to standard output");
            exit(EXIT_FAILURE);
        }
    }

    // Error handling for read()
    if (bytesRead == -1) {
        perror("Error reading from standard input");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
