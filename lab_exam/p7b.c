// C program to simulate copy command by accepting the filenames from the command line. 

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *src, *dest;
    char ch;

    if (argc != 3)
        return 1;

    src = fopen(argv[1], "r");
    dest = fopen(argv[2], "w");

    while ((ch = fgetc(src)) != EOF)
        fputc(ch, dest);

    fclose(src);
    fclose(dest);

    return 0;
}
