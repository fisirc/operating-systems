#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

typedef int pid_t;

struct pcb_t
{
  pid_t pid;
  const char *cmdline;
  struct list_elem children_elem;
  struct semaphore wait_sema;
  struct semaphore load_sema;
  bool exited;
  int exit_status;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /**< userprog/process.h */
