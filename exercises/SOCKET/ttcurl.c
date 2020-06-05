#include "main.h"
#include <sys/socket.h>  /* socket, connect */
#include <string.h>      /* memset */
#include <arpa/inet.h>   /* htons, inet_addr */
#include <sys/types.h>   /* read */
#include <sys/uio.h>     /* read */
#include <unistd.h>      /* read, write, close */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int write_body = 0;
int write_status = 0;
int sleeptime = 0;
int port = 10000;
char addr[1024] = "127.0.0.1";
char uri[1024] = "/";

void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "bcu:s:p:w:")) != -1) {
        switch (opt) {
        case 'b': /* -b: body */
            write_body = 1;
            break;
        case 'c': /* -c: status code */
            write_status = 1;
            break;
        case 's': /* -s SERVER: server */
            strcpy(addr, optarg);
            break;
        case 'u': /* -u URI: uri */
            strcpy(uri, optarg);
            break;
        case 'p': /* -p N: port number */
            port = atoi(optarg);
            if (port <= 0) DIE("Invalid port number");
            break;
        case 'w': /* -w N: sleep time */
            sleeptime = atoi(optarg);
            break;
        case '?':
        default:
            fprintf(stderr, "Usage: %s [-s server] [-p port]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {
    parse_options(argc, argv);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    FILE *fout = fdopen(sockfd, "wb");
    fprintf(fout, "GET %s HTTP/1.0\r\n", uri);
    if (sleeptime > 0) {
        sleep(sleeptime);
    }
    fprintf(fout, "\r\n");
    fflush(fout);
    shutdown(sockfd, SHUT_WR);

    FILE *fin = fdopen(sockfd, "rb");
    char buf[1024], version[1024];
    int status;

    // response line
    if (fgets(buf, sizeof(buf), fin) == NULL) return EXIT_FAILURE;
    sscanf(buf, "%s %d", version, &status);
    if (write_status) {
        printf("%d\n", status);
    }

    // header
    for (;;) {
        if (fgets(buf, sizeof(buf), fin) == NULL) return EXIT_FAILURE;
        if (strcmp(buf, "\r\n") == 0) break;
    }

    // body
    for (;;) {
        int n = fread(buf, 1, sizeof(buf), fin);
        if (n == 0) break;
        if (write_body) fwrite(buf, 1, n, stdout);
    }
    close(sockfd);

    return 0;
}
