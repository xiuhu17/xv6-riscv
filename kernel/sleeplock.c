// Sleeping locks

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

void
initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

/*
  similar to
    pthread_mutex_lock(); // lock mutex
    while(!cond) // !cond
      pthread_cond_wait(); // unlock mutex; lock mutex
    pthread_mutex_unlock(); // unlock mutex
  */
void
acquiresleep(struct sleeplock *lk)
{
  acquire(&lk->lk);  // lock spin
  while (lk->locked) {  // !cond
    sleep(lk, &lk->lk); // release &lk->lk; acquire &p->lock; mark sleeping, call sched(); release &p->lock; acquire &lk->lk // unlock spin; lock spin
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk); // unlock spin
}

/*
  similar to
    pthread_mutex_lock(); // lock mutex
    pthread_cond_wakeup();
    pthread_mutex_unlock(); // unlock mutex
  */
void
releasesleep(struct sleeplock *lk)
{
  acquire(&lk->lk); // lock spin
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk); // mark all sleeping process, wait on sleeplock *lk as runnable // notify
  release(&lk->lk); // unlock spin
}

int
holdingsleep(struct sleeplock *lk)
{
  int r;
  
  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}



