#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    struct procinfo info;
    int pid = (argc > 1) ? atoi(argv[1]) : getpid(); // Lấy PID từ tham số hoặc chính nó

    if(procinfo(pid, &info) == 0){
        printf("Process: %s\n", info.name);
        printf("PID: %d, PPID: %d\n", info.pid, info.ppid);
        printf("State: %d\n", info.state);
        printf("Memory: %ld bytes\n", info.sz);
    } else {
        printf("Error: Could not get info for PID %d\n", pid);
    }
    exit(0);
}