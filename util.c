//util.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int err_quit(char *msg, const char *file, const char *func, const int line) {
    printf("Error in function %s (%s, line %d): %s\n", func, file, line, msg);
    exit(1);
    return 0;
}

int readn(int fd, char *ptr, int nbytes) {
    int nleft, nread;
    for(nleft = nbytes; nleft > 0; ) {
        nread = read(fd, ptr, nleft);
        if(nread < 0)       //error
            return nread;
        else if(nread == 0) //EOF
            break;
        nleft -= nread;
        ptr   += nread;
    }
    return nbytes - nleft;
}

int writen(int fd, char *ptr, int nbytes) {
    int nleft, nwritten;
    for(nleft = nbytes; nleft > 0; ) {
        nwritten = write(fd, ptr, nleft);
        if(nwritten < 0)       //error
            return nwritten;

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return nbytes - nleft;
}

int readline(int fd, char *ptr, int maxlen) {
    int n, rc;
    char c;
    for(n = 1; n < maxlen; n++) { //read up to n-1 characters
        rc = read(fd, &c, 1);
        if(rc == 1) {
            *ptr++ = c;
            if(c == '\n')
                break;
        }
        else if(rc == 0) {
            if(n == 1)
                return 0;   //EOF, no data read
            else
                break;      //EOF, some data was read
        }
        else
            return -1;      //error
    }

    *ptr = 0; //add the null terminator
    return n;
}
