typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

struct procinfo {
    int pid; //Process id
    int ppid; // Parent process id
    int state; //Trạng thái (see kernel/proc.h)
    uint64 sz; //Kích thước bộ nhớ (bytes)
    char name[16]; //Tên tiến trình
};