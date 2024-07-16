#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{ // 将完整的路径名转化为固定长度的文件名
  static char buf[DIRSIZ + 1];
  char *p;

  // Find first character after last slash.
  for (p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if (strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
  return buf;
}

void ls(char *path)
{
  char buf[512], *p;
  int fd;           // 可以是文件的描述符，也可以是一个目录的描述符
  struct dirent de; // 用于描述目录inode的信息
  struct stat st;   // 用于描述底层文件inode的信息

  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type)
  {
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
      printf("ls: path too long\n");
      break;
    } // 当需要ls的路径过长时报错返回，因为内部缓冲区有限

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    // 在路径后面加上一个'/'

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      // 目录的描述符指向了一个存放了若干dirent的数组
      if (de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0)
      {
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  int i;

  if (argc < 2)
  {
    ls(".");
    exit(0);
  }
  for (i = 1; i < argc; i++)
    ls(argv[i]);
  exit(0);
}
