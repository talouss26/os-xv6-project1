#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//Viết hàm uint64 sys_trace(void) để lấy đối số từ người dùng bằng cách sử dụng argint và gán vào biến mask của tiến trình hiện tại(myproc()->trace_mask)
uint64 
sys_trace(void) 
{
  int mask;

  //Sử dụng argint để lấy tham số đầu tiên người dùng truyền vào
  argint(0, &mask);

  //Lưu mask này vào thông tin của tiến trình hiện tại
  myproc()->trace_mask = mask;
  return 0;
}

uint64
sys_procinfo(void)
{
  int pid;
  uint64 addr; // Địa chỉ con trỏ struct procinfo mà user truyền vào
  struct proc *p;
  struct procinfo temp_info;
  extern struct proc proc[NPROC]; // Tham chiếu đến bảng tiến trình của hệ thống

  // 1. Lấy 2 tham số từ User
  argint(0, &pid);
  argaddr(1, &addr);

  // 2. Duyệt bảng proc[] để tìm tiến trình có PID khớp
  int found = 0;
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock); // Khóa tiến trình để đảm bảo an toàn dữ liệu
    if(p->pid == pid){
      // 3. Thu thập thông tin vào biến tạm temp_info
      temp_info.pid = p->pid;
      temp_info.ppid = p->parent ? p->parent->pid : 0;
      temp_info.sz = p->sz;
      safestrcpy(temp_info.name, p->name, sizeof(p->name));

      // Chuyển trạng thái từ enum sang chuỗi (ví dụ: RUNNING)
      // Bạn có thể tham khảo mảng procstate[] trong kernel/proc.c
      // tạm thời giả định: safestrcpy(temp_info.state, "UNKNOWN", 16);

      found = 1;
      release(&p->lock);
      break;
    }
    release(&p->lock);
  }

  if(!found) return -1;

  // 4. COPY dữ liệu từ Kernel Space về User Space
  if(copyout(myproc()->pagetable, addr, (char *)&temp_info, sizeof(temp_info)) < 0)
    return -1;

  return 0;
}