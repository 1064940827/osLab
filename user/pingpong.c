#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc,char *argv[]){
    int p1[2],p2[2];
    int pid;
    char buf[1]={'o'};
    pipe(p1);//父进程读字节，子进程写字节
    pipe(p2);//父进程写字节，子进程读字节
    if(fork()!=0){
        //parent
        close(p1[1]);
        close(p2[0]);
        pid=getpid();

        write(p2[1],buf,1);
        read(p1[0],buf,1);
        fprintf(1,"%d: received pong\n",&pid);
        wait(0);
        exit(0);

    }else{
        //child
        close(p1[0]);
        close(p2[1]);
        pid=getpid();

        read(p2[0],buf,1);
        fprintf(1,"%d: received ping\n",&pid);
        write(p1[1],buf,1);
        exit(0);
    }
}