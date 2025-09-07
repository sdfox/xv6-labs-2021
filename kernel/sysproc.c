#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  backtrace();
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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

uint64
sys_sigalarm(void)
{
  int ticks;
  uint64 handler;

  if(argint(0, &ticks) < 0 || argaddr(1, &handler) < 0)
    return -1;

  struct proc *p = myproc();
  acquire(&tickslock);
  p->ticks = ticks;
  p->passed_ticks = 0;
  p->handler = (void*)handler;
  p->upcall_lock = 0;
  release(&tickslock);
  return 0;
}

uint64
sys_sigreturn(void)
{
  // recover the status of trapframe before upcall
  struct proc *p = myproc();
  p->trapframe->kernel_satp = p->upcall_reserve_trapframe.kernel_satp;
  p->trapframe->kernel_sp = p->upcall_reserve_trapframe.kernel_sp;
  p->trapframe->kernel_trap = p->upcall_reserve_trapframe.kernel_trap;
  p->trapframe->epc = p->upcall_reserve_trapframe.epc;
  p->trapframe->kernel_hartid = p->upcall_reserve_trapframe.kernel_hartid;
  p->trapframe->ra = p->upcall_reserve_trapframe.ra;
  p->trapframe->sp = p->upcall_reserve_trapframe.sp;
  p->trapframe->gp = p->upcall_reserve_trapframe.gp;
  p->trapframe->tp = p->upcall_reserve_trapframe.tp;
  p->trapframe->t0 = p->upcall_reserve_trapframe.t0;
  p->trapframe->t1 = p->upcall_reserve_trapframe.t1;
  p->trapframe->t2 = p->upcall_reserve_trapframe.t2;
  p->trapframe->s0 = p->upcall_reserve_trapframe.s0;
  p->trapframe->s1 = p->upcall_reserve_trapframe.s1;
  p->trapframe->a0 = p->upcall_reserve_trapframe.a0;
  p->trapframe->a1 = p->upcall_reserve_trapframe.a1;
  p->trapframe->a2 = p->upcall_reserve_trapframe.a2;
  p->trapframe->a3 = p->upcall_reserve_trapframe.a3;
  p->trapframe->a4 = p->upcall_reserve_trapframe.a4;
  p->trapframe->a5 = p->upcall_reserve_trapframe.a5;
  p->trapframe->a6 = p->upcall_reserve_trapframe.a6;
  p->trapframe->a7 = p->upcall_reserve_trapframe.a7;
  p->trapframe->s2 = p->upcall_reserve_trapframe.s2;
  p->trapframe->s3 = p->upcall_reserve_trapframe.s3;
  p->trapframe->s4 = p->upcall_reserve_trapframe.s4;
  p->trapframe->s5 = p->upcall_reserve_trapframe.s5;
  p->trapframe->s6 = p->upcall_reserve_trapframe.s6;
  p->trapframe->s7 = p->upcall_reserve_trapframe.s7;
  p->trapframe->s8 = p->upcall_reserve_trapframe.s8;
  p->trapframe->s9 = p->upcall_reserve_trapframe.s9;
  p->trapframe->s10 = p->upcall_reserve_trapframe.s10;
  p->trapframe->s11 = p->upcall_reserve_trapframe.s11;
  p->trapframe->t3 = p->upcall_reserve_trapframe.t3;
  p->trapframe->t4 = p->upcall_reserve_trapframe.t4;
  p->trapframe->t5 = p->upcall_reserve_trapframe.t5;
  p->trapframe->t6 = p->upcall_reserve_trapframe.t6;

  // clear sign
  p->upcall_lock = 0;
  return 0;
}
