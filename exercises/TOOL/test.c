#include <sys/types.h> /* open */
#include <sys/stat.h>  /* open */
#include <fcntl.h>     /* open */
#include <string.h>     /* len */
#include <sys/uio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int fd;
    if ((fd = open("cg.dot", O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1){
        perror("open");
        exit(1);
    }
    char *str = "strict digraph G {\n\n}";

    if (write(fd,str,strlen(str)) != strlen(str)){
        perror("write");
        exit(1);
    }
    for (int i=0 ; i<5; i++){
        lseek(fd, -2 , SEEK_END);
        char *str1 = "a -> b;\n\n}";
        if (write(fd,str1,strlen(str1)) != strlen(str1)){
            perror("write");
            exit(1);
        }
    }
    

    if (close(fd) == -1){
        perror("close");
        exit(1);
    }

    return 0;
}