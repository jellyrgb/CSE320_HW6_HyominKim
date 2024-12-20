//echo_server.c
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
// Include signal.h header file to use signal and SIGCHLD
#include <signal.h>
#include "util.h"

#define SERV_PORT   6000
#define MAXLINE     512

// Function to exit the parent process when a child process terminates
// This function is called when SIGCHLD signal is received
void handle_sigchld(int sig) {
    exit(0);
}

void echo(int fd) {
    while(1) {
        char line[MAXLINE];
        int n = readline(fd, line, MAXLINE);
        CHKBOOLQUIT(n >= 0, "readline error");

        // Handle invalid input from client
        if (n < 0) {
            printf("Invalid input from client. Terminating...\n");
            return;
        }

        //connection terminated
        if(n == 0) {
            return; 
        }

        // Handle exit command from client
        if (strcmp(line, "exit\n") == 0) {
            printf("Received exit command. Terminating...\n");
            break;
        }

        // Handle message longer than 50 from client
        if (strlen(line) > 50) {
            printf("Received message that is too long. Terminating...\n");
            break;
        }
        else {
            // Print received message from client
            printf("Client: %s", line);

            CHKBOOLQUIT(writen(fd, line, n) == n, "writen error");

            // Print server prompt and read message from server
            printf("You: ");
            fgets(line, sizeof(line), stdin);

            CHKBOOLQUIT(writen(fd, line, strlen(line)) == strlen(line), "writen error");

            // Terminate connection if exit command is sent
            if (strcmp(line, "exit\n") == 0) {
                printf("Terminating...\n");
                break;
            }

            // Handle message longer than 50 from server
            if (strlen(line) > 50) {
                printf("Message is too long! Terminating...\n");
                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    // Set SIGCHLD signal handler to handle child process termination
    signal(SIGCHLD, handle_sigchld);

    int sfd;
    struct sockaddr_in saddr;

    CHKBOOLQUIT( (sfd = socket(AF_INET, SOCK_STREAM, 0)) >= 0, "socket failed" );
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(SERV_PORT);
    CHKBOOLQUIT( bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) >= 0, "bind failed");
    CHKBOOLQUIT( listen(sfd, 1024) >= 0, "listen failed" );

    printf("Server listening...\n");

    while(1) {
        struct sockaddr_in caddr;
        int cfd;
	unsigned int clen = sizeof(caddr);
        CHKBOOLQUIT( (cfd = accept(sfd, (struct sockaddr*) &caddr, &clen)) >= 0,
                                                                       "accept failed");
        if(fork() == 0) {   //child
            close(sfd);

            // Handle invalid client IP address
            if (inet_ntoa(caddr.sin_addr) == NULL) {
                perror("Invalid client IP address.");
                close(cfd);
                continue;
            }

            printf("pid: %d, client: %s:%d\n", getpid(), inet_ntoa(caddr.sin_addr), caddr.sin_port);
            echo(cfd);
            printf("pid: %d done\n", getpid());
            exit(0);
        }
        else 
            close(cfd);
    }
    return 0;
}

