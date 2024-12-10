#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <threads/io.h>
#include <devices/shutdown.h>
#include <filesys/file.h>
#include <threads/vaddr.h>

static void syscall_handler (struct intr_frame *);

void*
validate_addr(void* addr, uint8_t bytesize)
{
    uint8_t* byte_aligned_addr = (uint8_t*) addr;

    if (byte_aligned_addr == NULL ||
        !is_user_vaddr(byte_aligned_addr) ||
        ((void*) byte_aligned_addr) >= PHYS_BASE
    ){
        thread_current()->pcb.xstat = -1;
        thread_exit();
        return NULL;
    }

    if (bytesize == 0)
        return addr;

    uint8_t* byte_aligned_addr_padded = ((uint8_t*) addr) + bytesize;

    if (byte_aligned_addr_padded == NULL ||
        !is_user_vaddr(byte_aligned_addr_padded) ||
        ((void*) byte_aligned_addr_padded) >= PHYS_BASE
    ) {
        thread_current()->pcb.xstat = -1;
        thread_exit();
        return NULL;
    }

    return addr;
}

#define get_valid(addr, type) *(type*) validate_addr(addr, sizeof(type))

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
    uint32_t status = get_valid((uint32_t*) f->esp + 1, uint32_t);

    thread_current()->pcb.xstat = status;
    thread_current()->pcb.proc_stat = EXITED;
    thread_exit();
}

int
sys_write(struct intr_frame *f)
{
    void* esp = validate_addr(f->esp, 0);

    uint32_t fd = get_valid((uint32_t*) esp + 1, uint32_t);
    char* buf = get_valid((char**) esp + 2, char*);
    uint32_t size = get_valid((uint32_t*) esp + 3, uint32_t);

    f->eax = size;
    if (fd == 1) {
        putbuf((char*) buf, size);
    }

    return size;
}


static void
syscall_handler (struct intr_frame *f UNUSED)
{
    if (!validate_addr(f->esp, 0)) {
        thread_current()->pcb.xstat = -1;
        thread_exit();
        return;
    }

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
