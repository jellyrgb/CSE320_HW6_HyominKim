//echo_client.c
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "util.h"

#define SERV_ADDR   "127.0.0.1"
#define SERV_PORT   6000
#define MAXLINE     512

void copy(int sfd) {
    char sline[MAXLINE], rline[MAXLINE];

    // Print user prompt
    printf("You: ");
    
    while(fgets(sline, MAXLINE, stdin) != NULL) {
        int n = strlen(sline);

        // Handle invalid input from user
        if (n <= 0) {
            printf("Invalid input! Terminating...\n");
            break;
        }

        // Handle exit command from user
        if (strcmp(sline, "exit\n") == 0) {
            // Send exit command to server
            CHKBOOLQUIT( writen(sfd, sline, n) == n, "writen error" );
            printf("Terminating...\n");
            break;
        }

        // Handle message longer than 50 from user
        if (n > 50) {
            // Send message to server
            CHKBOOLQUIT( writen(sfd, sline, n) == n, "writen error" );
            printf("Message is too long! Terminating...\n");
            break;
        }

        CHKBOOLQUIT( writen(sfd, sline, n) == n, "writen error" );

        CHKBOOLQUIT( (n = readline(sfd, rline, MAXLINE)) >= 0, "readline error" );
        
        // Read message from server
        int rn = readline(sfd, rline, MAXLINE);
        CHKBOOLQUIT(rn >= 0, "readline error");

        // Handle connection termination
        if (rn == 0) {
            printf("Server closed connection.\n");
            break;
        }

        // Print server response
        printf("Server: %s", rline);

        // Terminate if exit command is received
        if (strcmp(rline, "exit\n") == 0) {
            printf("Received exit command. Terminating...\n");
            break;
        }

        // Handle message longer than 50 from server
        if (strlen(rline) > 50) {
            printf("Received message that is too long. Terminating...\n");
            break;
        }

        printf("You: ");
    }
    CHKBOOLQUIT( ferror(stdin) == 0, "cannot read file" );
}

int main(int argc, char **argv) {
    int sfd;
    struct sockaddr_in saddr;

    CHKBOOLQUIT( (sfd = socket(AF_INET, SOCK_STREAM, 0)) >= 0,
                 "socket failed" );

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;

    // Check if the server IP address is valid
    if (inet_addr(SERV_ADDR) == INADDR_NONE) {
        printf("Invalid server IP address.\n");
        exit(EXIT_FAILURE);
    }

    saddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
    saddr.sin_port = htons(SERV_PORT);

    CHKBOOLQUIT( connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr)) >= 0,
                 "connectfailed" );
    printf("server: %s:%d\n", inet_ntoa(saddr.sin_addr), saddr.sin_port);

    printf("Connected to the server.\n");

    copy(sfd);

    close(sfd);
    return 0;
}
