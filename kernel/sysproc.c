#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h" //结构体

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

//第 2 步：实现 sys_trace 函数本体
// 当用户在代码里调用 trace(mask) 时，内核需要有一个函数来接收并保存这个 mask。
uint64
sys_trace(void)
{
  int mask;
  // argint 是 xv6 用来安全读取用户态传来的整数参数的函数
  if(argint(0,&mask)<0)return -1; // 获取用户程序传入的数据
  // myproc() 获取当前正在运行的进程
  myproc()->kama_syscall_trace=mask; //设置调用进程的kama_syscall_trace掩码mask
  return 0;
}
uint64
sys_sysinfo(void){
  struct sysinfo info;//这个结构体此刻是存放在内核栈（Kernel Stack）上的。它是操作系统的绝对私有财产，外面的普通用户程序根本看不见、也摸不着它。
  kama_freebytes(&info.freemem);// 获取空闲内存
  kama_procnum(&info.nproc); //获取进程数量 填进去上面的结构体中

  //获取到用户空间的sysinfo结构体地址后，要把内核空间的sysinfo结构体的数据复制过去，这样用户程序才能拿到数据。

  //获取用户虚拟地址
  uint64 dstaddr;
  argaddr(0, &dstaddr);//请帮我拉开第 0 个抽屉（a0 寄存器），把里面的那个内存地址拿出来，赋值给 dstaddr 变量。”
  
  //从内核空间拷贝数据到用户空间
  if (copyout (myproc()->pagetable, dstaddr, (char*)&info, sizeof info) < 0)//虚拟内存与页表隔离。
    return -1;
  return 0;
}