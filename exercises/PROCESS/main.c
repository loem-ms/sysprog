#include "main.h"
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/** The max characters of one cmdline (incl. NULL terminator) */
#define MAX_LINE 8192

#define BIRD "\xF0\x9F\x90\xA6"

/** Whether or not to show the prompt */
int prompt = 1;

char *cmdname;

#define MAX_PIPE 10

/** Run a node and obtain an exit status. */
int invoke_node(node_t *node) {
    LOG("Invoke: %s", inspect_node(node));
    int status;
    switch (node->type) {
    case N_COMMAND:
        for (int i = 0; node->argv[i] != NULL; i++) {
            LOG("node->argv[%d]: \"%s\"", i, node->argv[i]);
        }

        /* Simple command execution (Task 1); implement here */
        if(fork() == 0){
            /* child process */
            execvp(node->argv[0],node->argv);
            perror(node->argv[0]);
            exit(1);
        }
        if (wait(&status) == (pid_t)-1){
            perror("wait");
            exit(1);
        }
        break;

    case N_PIPE: /* foo | bar */
        LOG("node->lhs: %s", inspect_node(node->lhs));
        LOG("node->rhs: %s", inspect_node(node->rhs));
        /* Simple comm and execution (Tasks 2 and 3); implement here */
        
        /* One pipe */
        /*
        
        int fd[2];
        pipe(fd);
        if(fork() == 0){
            dup2(fd[1],1);
            close(fd[0]); close(fd[1]);
            execvp(node->lhs->argv[0],node->lhs->argv);
            perror(node->lhs->argv[0]);
            exit(1);
        }else {
            if(fork()==0){
                dup2(fd[0],0);
                close(fd[0]); close(fd[1]);
                execvp(node->rhs->argv[0],node->rhs->argv);
                perror(node->rhs->argv[0]);
                exit(1);
            }else{
                close(fd[0]); close(fd[1]);
                if (wait(&status) == (pid_t)-1){
                    perror("wait");
                    exit(1);
                }
                if (wait(&status) == (pid_t)-1){
                    perror("wait");
                    exit(1);
                }
            }
        }
        */
        
        int fd[MAX_PIPE][2];
        int i=0;
        do{
            if(node->rhs!=NULL){
                pipe(fd[i]);
                //LOG("created");
            }
            
            if(fork()==0){
                /* child process */
                if(i==0){
                    dup2(fd[i][1],1);
                    close(fd[i][0]); close(fd[i][1]);
                    //LOG("first pipe%d",i);
                    //LOG("cmd:%s",node->lhs->argv[0]);
                    execvp(node->lhs->argv[0],node->lhs->argv);
                    perror(node->lhs->argv[0]);
                    exit(1);
                }else if(node->type==N_COMMAND){
                    dup2(fd[i-1][0],0);
                    close(fd[i-1][0]); close(fd[i-1][1]);
                    //LOG("final pipe%d",i-1);
                    //LOG("cmd:%s",inspect_node(node));
                    execvp(node->argv[0],node->argv);
                    perror(node->argv[0]);
                    exit(1);
                }else{
                    dup2(fd[i-1][0],0);
                    dup2(fd[i][1],1);
                    close(fd[i-1][0]);close(fd[i-1][1]);
                    close(fd[i][0]);close(fd[i][1]);
                    //LOG("middle pipe%d-%d",i-1,i);
                    //LOG("cmd:%s",node->lhs->argv[0]);
                    execvp(node->lhs->argv[0],node->lhs->argv);
                    perror(node->lhs->argv[0]);
                    exit(1);
                }
            }else if(i>0){
                //LOG("close");
                close(fd[i-1][0]);close(fd[i-1][1]);
            }
            i++;
            if(node->type == N_COMMAND) break;
            else node = node->rhs;
        }while(node->type==N_PIPE || node->type==N_COMMAND);
        
        for(int j = i;j>0;j--){
            wait(&status);
        }
        
        break;

    case N_REDIRECT_IN:     /* foo < bar */
    case N_REDIRECT_OUT:    /* foo > bar */
    case N_REDIRECT_APPEND: /* foo >> bar */
        LOG("node->filename: %s", node->filename);

        /* Redirection (Task 4); implement here */

        break;

    case N_SEQUENCE: /* foo ; bar */
        LOG("node->lhs: %s", inspect_node(node->lhs));
        LOG("node->rhs: %s", inspect_node(node->rhs));

        /* Sequential execution (Task A); implement here */

        break;

    case N_AND: /* foo && bar */
    case N_OR:  /* foo || bar */
        LOG("node->lhs: %s", inspect_node(node->lhs));
        LOG("node->rhs: %s", inspect_node(node->rhs));

        /* Branching (Task B); implement here */

        break;

    case N_SUBSHELL: /* ( foo... ) */
        LOG("node->lhs: %s", inspect_node(node->lhs));

        /* Subshell execution (Task C); implement here */

        break;

    default:
        assert(false);
    }
    return 0;
}

void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qp")) != -1) {
        switch (opt) {
        case 'q': /* -q: quiet */
            l_set_quiet(1);
            break;
        case 'p': /* -p: no-prompt */
            prompt = 0;
            break;
        case '?':
        default:
            fprintf(stderr, "Usage: %s [-q] [-p] [cmdline ...]\n", cmdname);
            exit(EXIT_FAILURE);
        }
    }
}

int invoke_line(char *line) {
    LOG("Input line='%s'", line);
    node_t *node = yacc_parse(line);
    if (node == NULL) {
        LOG("Obtained empty line: ignored");
        return 0;
    }
    if (!l_get_quiet()) {
        dump_node(node, stdout);
    }
    int exit_status = invoke_node(node);
    free_node(node);
    return exit_status;
}

int main(int argc, char **argv) {
    cmdname = argv[0];
    parse_options(argc, argv);
    if (optind < argc) {
        /* Execute each cmdline in the arguments if exists */
        int exit_status;
        for (int i = optind; i < argc; i++) {
            exit_status = invoke_line(argv[i]);
        }
        return exit_status;
    }

    for (int history_id = 1;; history_id++) {
        char line[MAX_LINE];
        if (prompt) {
            // printf("ttsh[%d]> ", history_id);
            printf("ttsh[%d] %s ", history_id, BIRD);
        }
        /* Read one line */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* EOF: Ctrl-D (^D) */
            return EXIT_SUCCESS;
        }
        /* Erase line breaks */
        char *p = strchr(line, '\n');
        if (p) {
            *p = '\0';
        }
        invoke_line(line);
    }
}
