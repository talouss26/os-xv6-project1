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
  uint64 addr;
  struct proc *p;
  struct procinfo temp_info;
  extern struct proc proc[NPROC];

  // 1. Lấy 2 tham số từ người dùng [cite: 170, 171, 172]
  argint(0, &pid);   // Tham số 1: pid
  argaddr(1, &addr); // Tham số 2: địa chỉ struct

  // 2. Tìm tiến trình có PID tương ứng trong bảng proc[]
  int found = 0;
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock);
    if(p->pid == pid){
      // 3. Copy dữ liệu từ struct proc sang struct procinfo [cite: 169, 174]
      temp_info.pid = p->pid;
      temp_info.ppid = p->parent ? p->parent->pid : 0; // Lấy PID của cha 
      temp_info.state = p->state; // Trạng thái tiến trình 
      temp_info.sz = p->sz;       // Kích thước bộ nhớ 
      safestrcpy(temp_info.name, p->name, sizeof(p->name));
      
      found = 1;
      release(&p->lock);
      break;
    }
    release(&p->lock);
  }

  if(!found) return -1; // Không tìm thấy PID yêu cầu

  // 4. Copy kết quả về User Space [cite: 172]
  if(copyout(myproc()->pagetable, addr, (char *)&temp_info, sizeof(temp_info)) < 0)
    return -1;

  return 0;
}