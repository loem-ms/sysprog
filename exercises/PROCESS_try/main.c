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

#define MAX_CMD 20

/** Whether or not to show the prompt */
int prompt = 1;

char *cmdname;

/* open file for redirect */
int open_file(char *filename, type_t io){
    int f;
    if(io == N_REDIRECT_IN){
        f = open(filename,O_RDONLY);
        if(f==-1){
            perror("open");
            exit(1);
        }
    }else if(io == N_REDIRECT_OUT){
        f = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0666);
        if(f==-1){
            perror("open");
            exit(1);
        }
    }else {
        f = open(filename,O_WRONLY|O_APPEND|O_CREAT,0666);
        if(f==-1){
            perror("open");
            exit(1);
        }
    }
    return f;
}


/** Run a node and obtain an exit status. */
int invoke_node(node_t *node) {
    LOG("Invoke: %s", inspect_node(node));
    int status;
    pid_t cpid;
    switch (node->type) {
    case N_COMMAND:
        for (int i = 0; node->argv[i] != NULL; i++) {
            LOG("node->argv[%d]: \"%s\"", i, node->argv[i]);
        }

        /* Simple command execution (Task 1); implement here */
        cpid = fork();
        if(cpid == -1){
            perror("fork");
            exit(1);
        }
        if(cpid == 0){
          execvp(node->argv[0], node->argv);
        }
        if(wait(&status)==(pid_t)-1){
          perror("wait");
          exit(1);
        }
        break;

    case N_PIPE: /* foo | bar */
        LOG("node->lhs: %s", inspect_node(node->lhs));
        LOG("node->rhs: %s", inspect_node(node->rhs));

        /* Simple command execution (Tasks 2 and 3); implement here */
        int fd[MAX_CMD][2];
        int i=0;
        int cnt=0;
        do{
            if(node->type==N_PIPE){
                pipe(fd[i]);
                cnt++;
            }
            cpid = fork();
            if(cpid == -1){
                perror("fork");
                exit(1);
            }
            if(cpid==0){
                /* child process */
                if(i==0){
                    dup2(fd[i][1],1);
                    close(fd[i][0]); close(fd[i][1]);      
                }else if(node->type!=N_PIPE){
                    dup2(fd[i-1][0],0);
                    close(fd[i-1][0]); close(fd[i-1][1]);
                }else{
                    dup2(fd[i-1][0],0); dup2(fd[i][1],1);
                    close(fd[i-1][0]);  close(fd[i-1][1]);
                    close(fd[i][0]);    close(fd[i][1]);
                }
                if(node->type==N_PIPE){
                    node = node->lhs;
                    //LOG("node: %s",inspect_node(node));                 
                }
                if(node->type!=N_COMMAND){
                    /* redirect */
                    int fd = open_file(node->filename,node->type);
                    if(node->type==N_REDIRECT_IN) dup2(fd,0);
                    else dup2(fd,1);
                    close(fd);
                }
                execvp(*node_argv(node),node_argv(node));
            }else {
                //sleep(1);
                close(fd[i-1][0]);close(fd[i-1][1]);
            }
            i++;
            if(node->type!=N_PIPE) break;
            else node = node->rhs;
        }while(i<MAX_CMD);
        if(i>=MAX_CMD){
            fprintf(stderr,"too many commands");
            exit(1);
        }
        for(int j = i;j>0;j--){
            if(wait(&status)==(pid_t)-1){
                perror("wait");
                exit(1);
            }
        }
        break;

    case N_REDIRECT_IN:     /* foo < bar */
    case N_REDIRECT_OUT:    /* foo > bar */
    case N_REDIRECT_APPEND: /* foo >> bar */
        LOG("node->filename: %s", node->filename);

        /* Redirection (Task 4); implement here */
        cpid = fork();
        if(cpid == -1){
            perror("fork");
            exit(1);
        }
        if(cpid == 0){
            int fd = open_file(node->filename,node->type);
            if(node->type==N_REDIRECT_IN) dup2(fd,0);
            else dup2(fd,1);
            close(fd);

            LOG("node->lhs:%s",inspect_node(node->lhs));
            if(node->lhs->type==N_SUBSHELL){
                /* for subshell */
                _exit(invoke_node(node->lhs));
            }

            if(node->lhs->type!=N_COMMAND){
                /* redirect if exists */
                node=node->lhs;
                int fd = open_file(node->filename,node->type);
                if(node->type==N_REDIRECT_IN) dup2(fd,0);
                else dup2(fd,1);
                close(fd);
            }
            /* LHS */
            execvp(*node_argv(node),node_argv(node));
            perror(*node_argv(node));
            exit(1);
        }
        if (wait(&status) == (pid_t)-1){
            perror("wait");
            exit(1);
        }
        break;

    case N_SEQUENCE: /* foo ; bar */
        LOG("node->lhs: %s", inspect_node(node->lhs));
        LOG("node->rhs: %s", inspect_node(node->rhs));

        /* Sequential execution (Task A); implement here */
        while(node->type==N_SEQUENCE){
            cpid = fork();
            if(cpid == -1){
                perror("fork");
                exit(1);
            }
            if(cpid == 0){
                if(node->lhs->type!=N_COMMAND){
                    /* for subshell */
                    _exit(invoke_node(node->lhs));
                }else{
                    execvp(*node_argv(node),node_argv(node));
                }
            }else{
                if(wait(&status)==(pid_t)-1){
                    perror("wait");
                    exit(1);
                }
            }
            node = node -> rhs;
        }
        cpid = fork();
        if(cpid == -1){
            perror("fork");
            exit(1);
        }
        if(cpid==0){
            if(node->type!=N_COMMAND){
                /* for subshell */
                _exit(invoke_node(node));
            }else{
                execvp(node->argv[0],node->argv);
            }   
        }else{
            if(wait(&status)==(pid_t)-1){
                perror("wait");
                exit(1);
            }
        }
        break;

    case N_AND: /* foo && bar */
    case N_OR:  /* foo || bar */
        LOG("node->lhs: %s", inspect_node(node->lhs));
        LOG("node->rhs: %s", inspect_node(node->rhs));
        
        /* Branching (Task B); implement here */
        i = 0;
        node_t *backnode = make_node(node->type,NULL,node->rhs,NULL,NULL);
        while(node->type==N_OR || node->type==N_AND){
            if(i>0) {
                node_t *tmp = make_node(node->type,NULL,node->rhs,NULL,NULL);
                tmp->lhs = backnode;
                backnode = tmp;
            }
            node = node->lhs;
            i++;
        }
        if(fork()==0){
            execvp(node->argv[0],node->argv);
        }
        wait(&status);
        int exitstate;
        do{
            if(WIFEXITED(status)){
                exitstate=WEXITSTATUS(status);
            }else{
                perror("WIFEXITED"); exit(1);
            }
            LOG("exitstate: %d",exitstate);
            if(exitstate == ((backnode->type==N_AND)?0:1)){
                // from exit 0
                if(fork()==0){
                    // exec backnode->rhs
                    execvp(backnode->rhs->argv[0],backnode->rhs->argv);
                }else{
                    // parent: ttsh
                    wait(&status);
                }
            }else{
                // from exit 1
                ;
            }
            if(backnode->lhs == NULL) break;
            else backnode=backnode->lhs;
        }while(1);

        break;

    case N_SUBSHELL: /* ( foo... ) */
        LOG("node->lhs: %s", inspect_node(node->lhs));

        /* Subshell execution (Task C); implement here */
        if(fork()==0){
                _exit(invoke_node(node->lhs));
        }else{
            if(wait(&status)==(pid_t)-1){
                perror("wait");
                _exit(1);
            }
        }
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
        LOG("read");
        /* Erase line breaks */
        char *p = strchr(line, '\n');
        if (p) {
            *p = '\0';
        }
        LOG("line:%s",line);
        invoke_line(line);
    }
}
