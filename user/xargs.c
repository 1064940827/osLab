#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define END_OF_ARGT_LIST -1
#define ARGT_TOO_LONG 1
#define ARGT_READ_FINISH 0

int readArgt(char *buf)
{
    char temp, *p;
    int i;
    for (i = 0, p = buf; i < 512; i++)
    {
        if (read(0, &temp, 1))
        {
            if (temp == '\n')
            {
                *p = 0;
                return ARGT_READ_FINISH;
            }
            else
                *p++ = temp;
        }
        else
        {
            *p = 0;
            return END_OF_ARGT_LIST;
        }

        // 在读取到输入的参数列表末尾时，buf中仍有一个需要处理的参数
    }
    return ARGT_TOO_LONG;
}

int main(int argc, char *argv[])
{
    static char buf[512];
    char *newArgv[MAXARG];
    int state;
    int i;
    if (argc < 2)
    {
        printf("Usage: xargs <command> [arguments]\n");
        exit(1);
    }

    // xargs指令一般与管道同时使用
    // 由于管道的存在，管道左侧指令的执行结果被输入到了管道写端(重定向为标准输出)
    // 管道右侧指令执行时的标准输入被重定向为了管道读端

    for (i = 1; i < argc; i++)
        newArgv[i - 1] = argv[i];

    while (!(state = readArgt(buf)))
    {
        newArgv[argc - 1] = buf;
        if (fork() == 0)
        {
            // 子进程中
            exec(argv[1], newArgv);
            printf("exec %s error!\n", argv[0]);
            exit(1);
        }
        else
            wait(0); // 父进程等待子进程执行指令完毕
    }
    if (state == -1)
    {
        // 执行最后一条参数
        newArgv[argc] = buf;
        if (fork() == 0)
        {
            // 子进程中
            exec(argv[0], newArgv);
            printf("exec %s error!\n", argv[0]);
            exit(1);
        }
        else
            wait(0); // 父进程等待子进程执行指令完毕
        exit(0);
    }
    else if (state == 1)
    {
        printf("xargs's input argument too long! \n");
        exit(1);
    }
    return 0;
}