#include "main.h"
#include <fcntl.h>     /* open */
#include <stdio.h>     /* fopen, fread, fwrite, fclose */
#include <stdlib.h>    /* exit, atoi */
#include <sys/types.h> /* read */
#include <sys/uio.h>   /* read, write */
#include <unistd.h>    /* getopt, read, write, close */
#include <string.h>

/** argv[0] */
char *cmdname;

/* use stdio (-l option) */
int use_stdio = 0;

/* use stdin (use stdin) */
int use_stdin = 0;

/* buffer size (-s N option) */
unsigned int buffer_size = 64;

int do_cat(int size, char **args) {
    /* TaskA implement */
    if (size == 0){
      size = 1;
      *args = "stdin";
      use_stdin = 1;
    }

    for (int i = 0; i < size; i++) {
        char *fname = args[i];
        /* TaskA implement */
        if(!strcmp("-",fname)) {
          fname = "stdin";
          use_stdin = 1;
        }
        LOG("filename = %s", fname);
        if (!use_stdio) {
          /* -l unavailable (Task 1); implement here */
          int infd;
          ssize_t cc;
          char buf[buffer_size+1];

          if (use_stdin) infd = STDIN_FILENO;
          else{
            if((infd = open(fname,O_RDONLY)) == -1){
              perror("open input file");
              exit(1);
            }
          }
          
          while((cc = read(infd,buf,buffer_size)) > 0){
            if(write(STDOUT_FILENO,buf,cc) < 0){
              perror("write");
              exit(1);
            }
          }
          if(cc<0){
            perror("read input file");
            exit(1);
          }
          close(infd);
          //write(STDOUT_FILENO, "Hello World.\n", 13);
        } else {
          /* -l available (Task 2); implement here */
          FILE *infp;
          size_t cc;
          char buf[buffer_size+1];
          
          /* TaskA implement */
          if (use_stdin) infp = stdin;
          else{
            if((infp = fopen(fname,"r")) == NULL){
              fprintf(stderr,"fopen input file");
              exit(1);
            }
          }
          
          while((cc = fread(buf,sizeof(char),buffer_size,infp)) == buffer_size){
            if(fwrite(buf,sizeof(char),buffer_size,stdout) < buffer_size){
              fprintf(stderr, "fwrite");
              exit(1);
            }
          }
          if(feof(infp)){
            if(fwrite(buf,sizeof(char),cc,stdout) < cc){
              fprintf(stderr, "fwrite");
              exit(1);
            }
          }
          if(ferror(infp)){
            fprintf(stderr, "fread");
            exit(1);
          }
          fclose(infp);
        }
    }
    return 0;
}

/** parse program arguments. */
void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qls:")) != -1) {
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
