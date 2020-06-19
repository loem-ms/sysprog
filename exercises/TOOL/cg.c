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
#define MAX_CALLS 1024*2

int isRoot = 0;
int current_call = 0;
void *root_addr;
FILE *fp;
char *cg_label_env;

int called_idx = 0;
int called_num[MAX_CALLS] = {0};

void * call_log[MAX_DEPTH];
void * called_uniq[MAX_CALLS];

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
void open_file(){
    if ((fp = fopen("cg.dot", "w")) == NULL) {
        perror("fopen");
        exit(1);
    }
    char *str = "strict digraph G {\n\n}";
    fprintf(fp,"%s",str);
    isRoot = 1;
}

__attribute__((no_instrument_function))
int isCalled(void *addr, void *list[]) {
    int i=0;
    while (list[i]!= NULL && i<MAX_CALLS) {
        if (list[i] == addr) return i;
        i++;
    }
    return -1;
}



__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *addr, void *call_site) {
    LOG(">>> %s \n", addr2name(addr), addr);
    /* My Implementation */
    if (isRoot == 0) {
        open_file();
        call_log[current_call] = addr;
        root_addr = addr;
        cg_label_env = getenv("SYSPROG_LABEL");
        //LOG("env: %s\n",cg_label_env);
    } else {
        if (cg_label_env != NULL){
            int idx =isCalled(addr,called_uniq);
            if (idx == -1) {
                called_uniq[called_idx] = addr;
                LOG("added call: %s\n",addr2name(called_uniq[called_idx]));
                idx = called_idx;
                called_idx ++;
            }
            LOG("found at %d with label %d\n",idx,called_num[idx]);
            called_num[idx] += 1;
            fseek(fp,-2,SEEK_END);
            fprintf(fp,"%s -> %s [label=\"%d\"];\n}\n",addr2name(call_log[current_call]),addr2name(addr),called_num[idx]);
            current_call++;
            call_log[current_call] = addr;
            
        }else{
            fseek(fp,-2,SEEK_END);
            fprintf(fp,"%s -> %s;\n}\n",addr2name(call_log[current_call]),addr2name(addr));
            current_call++;
            call_log[current_call] = addr;
        }
    }
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *addr, void *call_site) {
    LOG("<<< %s (%p)\n", addr2name(addr), addr);
    /* My Implementation */
    call_log[current_call] = NULL;
    current_call--;
    if (addr == root_addr) {
        fclose(fp);
        isRoot = 0;
    }
}
