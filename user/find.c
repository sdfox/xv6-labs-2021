#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  /* open the dirent */
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s, no such file or directory\n", path);
    return;
  }

  /* read the dirent's info from file description symbol */
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    printf("find: %s is not a directory\n", path);
    exit(1);
  } else {
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      exit(1);
    }
    strcpy(buf, path); /* buf is the copy of path + '/' + de.name, so it is the full path of the file(?) */
    p = buf + strlen(buf);
    *p++ = '/';

    /* read items in dirent */
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0; /* p is the copy of de.name, it points to the char behind '/' in buf char arrays */
      if(de.inum == 0)
        continue;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
      } else if(strcmp(de.name, filename) == 0){ /* find the match file, print */
        printf("%s\n", buf);
      } else if(st.type == T_DIR /* recursive search the sub dir */
	          && (strcmp(de.name, ".") != 0)
		  && (strcmp(de.name, "..") != 0)
	       ){
	find(buf, filename);
      }
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "find: miss parameters");
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
}

