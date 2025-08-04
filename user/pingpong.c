#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  /* fdx[0] read end, fdx[1] write end */
  int fd1[2], fd2[2], pid;
  char buf[1];
  if(pipe(fd1) != 0 || pipe(fd2) != 0){
    fprintf(2, "pipe() failed\n");
    exit(1);
  }
  pid = fork();
  if(pid < 0){
    fprintf(2,"fork() failed\n");
    exit(1);
  } else if(pid > 0){ /* parent */
    close(fd1[0]);
    write(fd1[1], "1", 1);

    wait(0); /* wait child */

    close(fd2[1]);
    read(fd2[0], buf, 1);
    printf("%d: received pong\n", getpid());
    exit(0);
  } else { /* child */
    close(fd1[1]);
    read(fd1[0], buf, 1);
    printf("%d: received ping\n", getpid());

    close(fd2[0]);
    write(fd2[1], "2", 1);
    exit(0);
  }
}
