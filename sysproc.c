#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

int sys_fork(void)
{
  return fork();
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return proc->pid;
}

addr_t
sys_sbrk(void)
{
  addr_t addr;
  addr_t n;

  argaddr(0, &n);
  addr = proc->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (proc->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#define MMAP_FAILED ((~0lu))

addr_t
sys_mmap(void)
{
  int fd, flags;
  // Failed arg fetch for either 0th or 1st arg or max no. mmaps reached
  if (argint(0, &fd) < 0 || argint(1, &flags) < 0 || proc->mmapcount + 1 >= NMMAP)
  {
    cprintf("Failed arg fetch for either 0th or 1st arg or max no. mmaps reached");
    return MMAP_FAILED;
  }

  if (flags == 0)
  {
    // eager mode

    // allocate some amount of page tables to store contents into it
    // mmap grows from 0x400000000000 and above.
    // limit should be top of user memory? 0x700000000000??
    
    int mmap_index = proc->mmapcount;
    
    proc->mmaps[mmap_index].fd = fd;
    proc->mmaps[mmap_index].start = (addr_t) proc->mmaptop;
    proc->mmapcount++;

    struct file* f = proc->ofile[fd];

    int file_sz = PGROUNDUP(f->ip->size); // Round up so that all contents fit inside page tables
    char* mem;
    int r; // 

    for (addr_t a = 0; a < file_sz; a+= PGSIZE)
    {
      mem = kalloc();
      
      // clear page contents
      memset(mem, 0, PGSIZE); 
      if (mappages(proc->pgdir, proc->mmaptop, PGSIZE, V2P(mem), PTE_W | PTE_U) < 0)
      {
        cprintf("Error occured while mapping pages broh");
        kfree(mem);
        return MMAP_FAILED;
      }

      proc->mmaptop += PGSIZE; // increment mmap top by size

      // put contents of file into page table
      ilock(f->ip);
      if((r = readi(f->ip, mem, f->off, PGSIZE)) > 0)
      {
        f->off += r;
        cprintf(mem);
      }
      iunlock(f->ip);
    }

    return proc->mmaps[mmap_index].start;
  }
  else
  if (flags == 1)
  {
    // lazy mode
    int mmap_index = proc->mmapcount;
    
    proc->mmaps[mmap_index].fd = fd;
    proc->mmaps[mmap_index].start = (addr_t) proc->mmaptop;
    proc->mmapcount++;

    struct file* f = proc->ofile[fd];

    proc->mmaptop += PGROUNDUP(f->ip->size); // update mmaptop to account of size of file
    
    return proc->mmaps[mmap_index].start;
  }

  return MMAP_FAILED;
}

int handle_pagefault(addr_t va)
{
  // TODO: your code here

  // Gives the address where something was illegally accessed
  // Seg fault?
  cprintf("page fault \n");
  cprintf("%x", va);

  va = PGROUNDDOWN(va);

  for (int i = 0; i < NMMAP; ++i)
  {
    struct file* f = proc->ofile[proc->mmaps[i].fd];

    addr_t mmap_end = proc->mmaps[i].start + PGROUNDUP(f->ip->size);
    
    if (proc->mmaps[i].start <= va && va < mmap_end) // check if va is in range of mmap
    {
      char* mem;
      int r;

      mem = kalloc();
      memset(mem, 0, PGSIZE); // clear kernel page

      if ((r = mappages(proc->pgdir, (void*) va, PGSIZE, V2P(mem), PTE_W | PTE_U)) < 0)
      {
        // something went wrong while mapping page
        return 0;
      }

      f->off = mmap_end - va; // get offset by subtracting end by va & round down

      // put contents of file into page table
      ilock(f->ip);
      if((r = readi(f->ip, mem, f->off, PGSIZE)) > 0)
      {
        f->off += r;
      }
      iunlock(f->ip);

      return 1;
    }
  }

  // return 1 after doing the thing we needed to do

  return 0;
}
