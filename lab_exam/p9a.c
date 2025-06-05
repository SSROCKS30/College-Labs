// Write a C program to demonstrate the usage of umask and chmod functions.



// chmod.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
int main(){

    struct stat statbuf;
    stat("foo", &statbuf);
    chmod("foo", statbuf.st_mode & ~S_IXGRP);
    /* set absolute mode to "rw-r--r--" */
    chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    
    return 0;
}


// umask.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main(){
    umask(0);
    creat("foo", RWRWRW);
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    creat("bar", RWRWRW)
    
    return 0;
}