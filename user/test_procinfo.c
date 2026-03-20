#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    struct procinfo info;

    int pid = getpid();

    if(procinfo(pid, &info) == 0){
        printf("Name: %s\n", info.name);
        printf("PID: %d\n", info.pid);
        printf("PPID: %d\n", info.ppid);
        printf("State: %s\n", info.state);
        printf("Memory: %ld\n", info.sz);
    } else {
        printf("procinfo failed\n");
    }

    exit(0);
}