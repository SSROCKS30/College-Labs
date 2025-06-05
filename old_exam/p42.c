// Write a program to read n characters from a file and append them back to the same file using dup2 function.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    char filename[] = "input.txt";
    
    int fd = open(filename, O_RDWR);
    char buffer[1024];
    
    int bytes_read = read(fd, buffer, 10);
    close(fd);
    
    fd = open(filename, O_WRONLY | O_APPEND);
    dup2(fd, STDOUT_FILENO);
    printf("%.*s", bytes_read, buffer);
    close(fd);
    printf("\nSuccessfully appended the characters to the file.\n");
    return 0;
}
