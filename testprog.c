#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>
#include <ctype.h>

enum{
    READLEN = 16,
};

int main(int argc, char* argv[]){
    int fd;
    ssize_t cc;
    char buf[READLEN+1];
    off_t pos;

    if(argc != 2){
        fprintf(stdout,"input file not defined.");
        exit(1);
    }

    if((fd = open(argv[1],O_RDONLY)) == -1){
        perror("open file");
        exit(1);
    }
    if ((pos = lseek(fd, 0, SEEK_SET)) == -1){
		perror("lseek");
		exit(1);
	}
    while((cc = read(fd, buf, sizeof(buf))) > 0){
        printf("%#x\t",(int)pos);
        for(int i=0;i<READLEN+1;i++){
            printf("%x ",buf[i]);
        }
        printf("\t");
        for(int i=0;i<READLEN+1;i++){
            if(!(isprint(buf[i]))) printf("・");
            else printf("%c",buf[i]);
        }
        printf("\n");
        if ((pos = lseek(fd, 0, SEEK_CUR)) == -1){
		perror("lseek");
		exit(1);
	}
    }

    return 0;
}