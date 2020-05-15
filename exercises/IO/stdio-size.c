#include <stdio.h>
#include <stdlib.h>

#if defined(_IO_UNBUFFERED)

int is_unbuffered(FILE *fp) { return (fp->_flags & _IO_UNBUFFERED) != 0; }
int is_linebuffered(FILE *fp) { return (fp->_flags & _IO_LINE_BUF) != 0; }
int buffer_size(FILE *fp) { return fp->_IO_buf_end - fp->_IO_buf_base; }

#elif defined(__SNBF)

int is_unbuffered(FILE *fp) { return (fp->_flags & __SNBF) != 0; }
int is_linebuffered(FILE *fp) { return (fp->_flags & __SLBF) != 0; }
int buffer_size(FILE *fp) { return fp->_bf._size; }

#elif defined(_IONBF)

#ifdef _LP64
#define _flag __pad[4]
#define _ptr __pad[1]
#define _base __pad[2]
#endif

int is_stdio_unbuffered(FILE *fp) { return (fp->_flag & _IONBF) != 0; }
int is_linebuffered(FILE *fp) { return (fp->_flag & _IOLBF) != 0; }
int buffer_size(FILE *fp) {
#ifdef _LP64
    return fp->_base - fp->_ptr;
#else
    return BUFSIZ; // expectation
#endif
}

#else

#error unknown stdio implementation!

#endif

void check(FILE *fp) {
    printf("buffer size: %d\n", buffer_size(fp));
    printf("line buffered: %d\n", is_linebuffered(fp));
    printf("unbuffered: %d\n", is_unbuffered(fp));
}

/* TaskB implement */
int main(int argc, char **argv) {
    int size[8] = {4194304, 524288, 65536, 8192, 1024, 128, 16, 2};
    system("cat /dev/random | base64 | head -c 1048756 > 'random.bin' ");
    FILE *infp;
    if((infp = fopen("random.bin","r")) == NULL){
        fprintf(stderr,"fopen input file");
        exit(1);
    }

    for(int i=0;i<8;i++){
      int buffer_size = size[i];
      
      size_t cc;
      char buf[buffer_size+1];
    
      while((cc = fread(buf,sizeof(char),buffer_size,infp)) == buffer_size){
        if(fwrite(buf,sizeof(char),10,stdout) < 10){
            fprintf(stderr, "fwrite");
            exit(1);
          }
      }
      if(feof(infp)){
        if(fwrite(buf,sizeof(char),10,stdout) < 10){
          fprintf(stderr, "fwrite");
          exit(1);
        }
      }
      if(ferror(infp)){
        fprintf(stderr, "fread");
        exit(1);
      }
      fflush(stdout);
      puts("\n##########################");
      printf("+ %d \n",buffer_size);
      puts("----[infp]----");
      check(infp);
      puts("----[stdout]----");
      check(stdout); 
      puts("##########################");
    }
    fclose(infp);   
}
