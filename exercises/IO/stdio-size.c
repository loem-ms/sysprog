#include <stdio.h>
#include <stdlib.h>

#if defined(_IO_UNBUFFERED)

int is_unbuffered(FILE *fp) {
    return (fp->_flags & _IO_UNBUFFERED) != 0;
}
int is_linebuffered(FILE *fp) {
    return (fp->_flags & _IO_LINE_BUF) != 0;
}
int buffer_size(FILE *fp) {
    return fp->_IO_buf_end - fp->_IO_buf_base;
}

#elif defined(__SNBF)

int is_unbuffered(FILE *fp) {
    return (fp->_flags & __SNBF) != 0;
}
int is_linebuffered(FILE *fp) {
    return (fp->_flags & __SLBF) != 0;
}
int buffer_size(FILE *fp) {
    return fp->_bf._size; 
}

#elif defined(_IONBF)

#ifdef _LP64
  #define _flag __pad[4]
  #define _ptr __pad[1]
  #define _base __pad[2]
#endif

int is_stdio_unbuffered(FILE *fp) {
    return (fp->_flag & _IONBF) != 0;
}
int is_linebuffered(FILE *fp) {
    return (fp->_flag & _IOLBF) != 0;
}
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

int main(int argc, char **argv) {
    
    FILE *inf;
    size_t cc;
    char buf[8192];
    int count;    
    if((inf=fopen("random.bin","r"))==NULL){
        fprintf(stderr,"fopen input file");
        exit(1);
    }
    puts("opened");
    if((cc=fread(buf,1,sizeof(buf),inf))<sizeof(buf)){
        if(ferror(inf)){
            fprintf(stderr,"fopen input file");
            exit(1);
        }else{
            fwrite(buf,1,cc,stdout);
        }
    }
    puts("inf :");
    check(inf);
    fclose(inf);

    return 0;
}
