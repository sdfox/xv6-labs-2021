#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#define MAXLINE 32

/* slice a full input string to an argv array */
char** slice(const char *str){
  int i = 0, len = 0, argc = 0;
  char **argv, *start;
  argv = malloc(sizeof(void*));

  int str_len = 0;
  while(str[i++] != '\n'){
    str_len++;
  }
  str_len++;

  for(i = 0, start = str;i < str_len;i++){
    if(str[i] == ' ' || str[i] == '\n'){
      start++;
      continue;
    }

    len++;
    if(i + 1 <= str_len && (str[i+1] == ' ' || str[i+1] == '\n')){
      argv[argc] = malloc(len + 1);
      memmove(argv[argc], start, len);
      argc++;

      len = 0;
      start = &str[i+1];
    }
  }
  return argv;
}

/* get new argv array's length */
int get_argc(char *str){
  int i = 0, argc = 0;

  int str_len = 0;
  while(str[i++] != '\n'){
    str_len++;
  }
  str_len++;

  for(i = 0;i < str_len;i++){
    if(str[i] == ' ' || str[i] == '\n'){
      continue;
    }
    if(i + 1 <= str_len && (str[i+1] == ' ' || str[i+1] == '\n')){
      argc++;
    }
  }
  return argc;
}

/* merge two argv */
char** merge(int argc1, int argc2, char **argv1, char **argv2){
  int new_argc = argc1 + argc2;
  char **new_argv = malloc(sizeof(char*) * new_argc);

  int i, j;
  for(i = 0;i < argc1;i++){
    new_argv[i] = argv1[i];
  }
  for(j = 0;j < argc2;i++, j++){
    new_argv[i] = argv2[j];
  }
  return new_argv;
}

int
main(int argc, char**argv)
{
  int i = 0, len = 0, new_argc[MAXLINE];
  char buf[1024], **new_argv[MAXLINE];

  char *str[MAXLINE];
  int i_ = 0, temp_ = 0;
  char *start_ = buf;
  /* read the pipe data from standard input(1) */
  /* now still lack of array out of bounds check */
  while(read(0, &buf[i], 1) != 0){
    temp_++;
    if(buf[i] == '\n'){
      str[i_] = malloc(temp_);
      memmove(str[i_], start_, temp_);

      new_argc[i_] = get_argc(str[i_]);
      new_argv[i_] = slice(str[i_]);

      start_ = &buf[i + 1];
      temp_ = 0;
      i_++;
    }
    len++;
    i++;
  }

  for(int i = 0;i < i_;i++){
    int argc_ = argc - 1 + new_argc[i];
    char** argv_ = merge(argc - 1, new_argc[i], &argv[1], new_argv[i]);
    int pid;
    pid = fork();
    if(pid < 0){
      fprintf(2, "fork failed\n");
    } else if(pid > 0){
      wait(0);
    } else {
      exec(argv_[0], argv_);
    }
  }
  exit(0);
}
