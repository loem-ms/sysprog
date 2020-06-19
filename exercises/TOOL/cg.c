#define _GNU_SOURCE
#include <dlfcn.h>  /* dladdr */
#include <stdarg.h> /* va_list */
#include <stdio.h>  /* printf */
#include <stdlib.h> /* atexit, getenv */

#include <sys/types.h> /* open */
#include <sys/stat.h>  /* open */
#include <fcntl.h>     /* open */
#include <string.h>     /* len */
#include <sys/uio.h>
#include <unistd.h>

#define MAX_DEPTH 32
#define MAX_current_call 1024

int isRoot = 0;
int current_call = 0;
void *root_addr;
FILE *fp;

void * call_addr[MAX_DEPTH];

__attribute__((no_instrument_function))
int log_to_stderr(const char *file, int line, const char *func, const char *format, ...) {
    char message[4096];
    va_list va;
    va_start(va, format);
    vsprintf(message, format, va);
    va_end(va);
    return fprintf(stderr, "%s:%d(%s): %s\n", file, line, func, message);
}
#define LOG(...) log_to_stderr(__FILE__, __LINE__, __func__, __VA_ARGS__)

__attribute__((no_instrument_function))
const char *addr2name(void* address) {
    Dl_info dli;
    if (dladdr(address, &dli) == 0) return NULL;
    return dli.dli_sname;
}

__attribute__((no_instrument_function))
void isRoot_cg_dot(){
    if ((fp = fopen("cg.dot", "w")) == NULL) {
        perror("fopen");
        exit(1);
    }
    char *str = "strict digraph G {\n\n}";
    fprintf(fp,"%s",str);
    isRoot = 1;
}




__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *addr, void *call_site) {
    /* My Implementation */
    if (isRoot == 0) {
        isRoot_cg_dot();
        call_addr[current_call] = addr;
        root_addr = addr;
    } else {
        fseek(fp,-2,SEEK_END);
        fprintf(fp,"%s -> %s;\n}\n",addr2name(call_addr[current_call]),addr2name(addr));
        current_call++;
        call_addr[current_call] = addr;
    }
    LOG(">>> %s \n", addr2name(addr), addr);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *addr, void *call_site) {
    /* My Implementation */
    call_addr[current_call] = NULL;
    current_call--;
    LOG("<<< %s (%p)\n", addr2name(addr), addr);
    if (addr == root_addr) fclose(fp);
}
