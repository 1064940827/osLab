#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void new_proc(int p[2])
{
    int primeNum = 0;
    int numberTemp, sonCreate = 0;
    int newp[2];
    close(p[1]);
    if (!primeNum)
    {
        // 进程首次创建,打印其对应的素数
        read(p[0], &primeNum, sizeof(int));
        // fprintf(1, "素数筛%d创建\n");

        fprintf(1, "prime %d\n", primeNum);
    }
    while (read(p[0], &numberTemp, sizeof(int)))
    {
        if (numberTemp % primeNum)
        {
            if (!sonCreate)
            {
                // 当未产生子进程时且出现了能通过此进程筛的数字，生成一个新进程以及连接新进程的管道
                pipe(newp);
                if (fork() != 0)
                {
                    // 当前进程
                    sonCreate = 1;
                    close(newp[0]);
                    write(newp[1], &numberTemp, sizeof(int));
                }
                else
                {
                    // 创建子进程
                    new_proc(newp);
                }
            }
            else
            {
                write(newp[1], &numberTemp, sizeof(int));
            }
        }
    }
    close(p[0]);
    close(newp[1]);
    wait(0);
    // fprintf(1, "%d exit\n", primeNum);
    exit(0);
}

int main(int argc, char *argv[])
{
    int pFeed[2];
    pipe(pFeed);
    if (fork() != 0)
    {
        close(pFeed[0]);
        for (int i = 2; i <= 35; i++)
        {
            // fprintf(1, "input%d\n", i);
            write(pFeed[1], &i, sizeof(int));
        }
        close(pFeed[1]);
        wait(0);
        exit(0);
    }
    else
    {
        new_proc(pFeed);
    }
    return 0;
}