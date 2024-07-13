#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *getFileName(char *path)
{
    char *p = path + strlen(path);
    while (p >= path && (*p) != '/')
        p--;

    return p + 1;
}

void find(char *path, char *findName)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        close(fd);
        exit(1);
    }

    switch (st.type)
    {
    case T_FILE:
        printf("The input path is a file!\n");
        break;
    case T_DIR:
        // 此时fd所指向的是一个存放了若干dirent的数组

        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
        {
            printf("find: path too long!\n");
            break;
        }

        strcpy(buf, path);
        p = buf + strlen(path);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                // 获取搜索目录下的文件/目录信息
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            switch (st.type)
            {
            case T_FILE:
                if (!strcmp(getFileName(buf), findName))
                    printf("%s\n", buf);
                break;
            case T_DIR:
                find(buf, findName);
                break;
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: find <path> <findName>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}