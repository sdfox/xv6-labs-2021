#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int read_fd){
  int p, n, pid, pfd[2];
  if(read(read_fd, &p, 4) == 0){
    close(read_fd);
    return;
  }
  printf("prime %d\n", p);

  if(pipe(pfd) != 0){
    fprintf(2, "pipe() failed\n");
    exit(1);
  }

  pid = fork();
  if(pid < 0){
    fprintf(2, "fork() failed\n");
  } else if(pid > 0){ /* parent */
    close(pfd[0]);
    while(read(read_fd, &n, 4) != 0){
      if(n % p != 0){
        write(pfd[1], &n, 4);
      }
    }
    close(read_fd);
    close(pfd[1]);
    wait(0);
    exit(0);
  } else { /* child */
    close(pfd[1]);
    close(read_fd);
    primes(pfd[0]);
    exit(0);
  }
}

int
main(int argc, char **argv)
{
  int fd[2], pid, i;
  if(pipe(fd) != 0){
    fprintf(2, "pipe() failed\n");
  }
  pid = fork();
  if(pid < 0){
    fprintf(2, "fork() failed\n");
  } else if(pid > 0){ /* parent */
    close(fd[0]);
    for(i=2; i<=35;i++){
      write(fd[1], &(int){i}, 4);
    }
    close(fd[1]); /* Close write end before waiting */
    wait(0); /* wait for all children */
    exit(0);
  } else if(pid == 0){ /* child */
    close(fd[1]);
    primes(fd[0]);
    exit(0);
  }
}

