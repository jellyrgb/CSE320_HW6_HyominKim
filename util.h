//util.h
#ifndef __UTIL__
#define __UTIL__

#define ERRQUIT(msg)         (err_quit((msg), __FILE__, __func__, __LINE__))
#define CHKBOOLQUIT(exp, msg)((exp) || ERRQUIT(msg))
extern int err_quit(char *msg, const char *file, const char *func,
                    const int line);
extern int readn(int fd, char *ptr, int nbytes);
extern int writen(int fd, char *ptr, int nbytes);
extern int readline(int fd, char *ptr, int maxlen);
#endif
