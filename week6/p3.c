#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
int main(int argc, char *argv[]){
        char buf[5000];
        int fd1,fd2;
        off_t size,ret,set;
        ssize_t readdata,writedata;
        if(argc<3)
                printf("TOO FEW ARGUMENTS");

        fd1=open(argv[1],O_RDONLY);
        if(fd1==-1)
                printf("ERROR IN OPENING FILE: FILE DOES NOT EXIST \n");
        else
                printf("FILE 1 OPENED SUCCESSFULLY \n");



        fd2=open(argv[2],O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if(fd2==-1)
                printf("ERROR IN OPENING FILE");
        else
                printf("FILE 2 OPENED SUCCESSFULLY \n");



        size=lseek(fd1,0L,SEEK_END);
        if(size==-1)
                printf("ERROR: COULD NOT OBTAIN FILE SIZE \n");
        else
                printf("FILE SIZE OF FILE 1 OBTAINED \n");


        ret=lseek(fd1,0L,SEEK_SET);
        if(ret==-1)
                printf("RETRACE FAILED \n");

        readdata=read(fd1,buf,size);
        if(readdata==-1)
                printf("ERROR IN READING FILE CONTENTS \n");

        writedata=write(fd2,buf,size);
        if(writedata!=size)
                printf("ERROR IN COPYING FILE");
        else
                printf("FILE COPIED SUCCESSFULLY");
        return 0;
}
