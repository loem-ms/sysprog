#include "main.h"
#include <fcntl.h>     /* open */
#include <stdio.h>     /* fopen, fread, fwrite, fclose */
#include <stdlib.h>    /* exit, atoi */
#include <sys/types.h> /* read */
#include <sys/uio.h>   /* read, write */
#include <unistd.h>    /* getopt, read, write, close */

/** argv[0] */
char *cmdname;

/* use stdio (-l option) */
int use_stdio = 0;

/* buffer size (-s N option) */
unsigned int buffer_size = 64;

/* use stdin */
int use_stdin = 0;

int do_cat(int size, char **args) {

    for (int i = 0; i < size; i++) {
        
        char *fname = args[i];
        LOG("filename = %s", fname);
        
        if (!use_stdio) {
            /* -l unavailable (Task 1); implement here */
            /* myImplement ⬇*/
            int infd;
            ssize_t cc;
            char buf[buffer_size];
            
            if((infd=open(fname,O_RDONLY))==-1){
                perror("open input file");
                exit(1);
            }
            
            while((cc=read(infd,buf,sizeof(buf)))>0){
                if((write(STDOUT_FILENO,buf,cc))!=cc){
                    perror("write");
                    exit(1);
                }
            }
            if(cc==-1){
                perror("read input file");
                exit(1);
            }
            close(infd);
            
            /* myImplement ⬆*/ 
            //write(STDOUT_FILENO, "Hello World.\n", 13);
        } else {
            /* -l available (Task 2); implement here */
            /* myImplement ⬇*/

            FILE *inf;
            size_t cc;
            char buf[buffer_size];
            int count;

            if((inf=fopen(fname,"r"))==NULL){
                fprintf(stderr,"fopen input file");
                exit(1);
            }

            while((cc = fread(buf,sizeof(char),sizeof(buf),inf))>0){
                if(ferror(inf)){
                    fprintf(stderr,"fread error");
                    exit(1);
                }else{
                    if(feof(inf)) count = cc;
                    else count = sizeof(buf);
                    if((fwrite(buf,sizeof(char),count,stdout))<count){
                        fprintf(stderr,"fwrite error");
                        exit(1);
                    }
                }
            }
            fclose(inf);
            /* myImplement ⬆*/ 
        }
    }
    return 0;
}

/** parse program arguments. */
void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qfls:")) != -1) {
        switch (opt) {
        case 'q': /* -q: quiet */
            l_set_quiet(1);
            break;
        case 'l': /* -l: use stdio */
            use_stdio = 1;
            break;
        case 's': /* -s N: set buffer size */
            buffer_size = atoi(optarg);
            if (buffer_size == 0) {
                DIE("Invalid size");
            }
            break;
        case '?':
        default:
            fprintf(stderr, "Usage: %s [-q] [-l] [-s size] file ...\n",
                    cmdname);
            exit(EXIT_FAILURE);
        }
    }
}

/** the entrypoint */
int main(int argc, char **argv) {
    cmdname = argv[0];
    parse_options(argc, argv);
    return do_cat(argc - optind, argv + optind);
}
