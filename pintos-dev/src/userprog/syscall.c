#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <threads/io.h>
#include <devices/shutdown.h>
#include <filesys/file.h>

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void
sys_halt(struct intr_frame *f UNUSED)
{
    DEBUGPRINT("shutdown!!!!\n");
    shutdown_power_off();
    NOT_REACHED();
}

void
sys_exit(struct intr_frame *f)
{
    uint32_t status = *((uint32_t*) f->esp + 1);

    thread_current()->pcb.xstat = status;
    thread_current()->pcb.proc_stat = EXITED;
    thread_exit();
}

int
sys_write(struct intr_frame *f)
{
    void* esp = f->esp;
    uint32_t fd = *((uint32_t*) esp + 1);
    char* buf = *((char**) esp + 2);
    uint32_t size = *((uint32_t*) esp + 3);

    f->eax = size;
    if (fd == 1) {
        putbuf((char*) buf, size);
    }

    return size;
}


static void
syscall_handler (struct intr_frame *f UNUSED)
{
    enum syscall_code code = *(enum syscall_code*) f->esp;

    switch (code) {
        case SYS_HALT:
            sys_halt(f);
            thread_exit ();
            break;
        case SYS_EXIT:
            sys_exit(f);
            thread_exit ();
            break;
        case SYS_WRITE:
            sys_write(f);
            break;
        default:
            break;
    }

    return;
}
