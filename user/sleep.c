#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char* argv[]){
    if(argc<2){
        fprintf(2,"Usage: sleep secs....\n");
        exit(1);
    }
    int sleepTime;
    if((sleepTime=atoi(argv[1]))<=0){
        fprintf(2,"The sleep time should be a positive integer!\n");
        exit(1);
    }
    sleep(sleepTime);
    exit(0);
}