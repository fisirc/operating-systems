# Threads

## Thread anatomy

This is what a kernel thread looks like:

```
        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+
```

All threads are initialized inside their own 4kiB page, mos of which is used to
hold the kernel stack, the kernel header (the actual structure) is allocated at
the head of that page (offset `0x00`).

There are 2 ways of scheduling processes in kernel threads as of right now:

1. Round-robin scheduling
1. MLFQS (Multi-Level Feedback Queue Scheduler)

By themselves, this is what thread structs look like:

``` c
struct thread {
    /* Owned by thread.c. */
    tid_t tid;                          /**< Thread identifier. */
    enum thread_status status;          /**< Thread state. */
    char name[16];                      /**< Name (for debugging purposes). */
    uint8_t *stack;                     /**< Saved stack pointer. */
    int priority;                       /**< Priority. */
    struct list_elem allelem;           /**< List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /**< List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /**< Page directory. */
#endif

    /* Owned by thread.c. */
    uint32_t magic;                     /**< Detects stack overflow. */
};
```

It all starts with `thread_init()` and `thread_start()` apparently, this is what
they do under the hood:

```c
/*
 * Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */
void
thread_init (void)
{
    ASSERT (intr_get_level () == INTR_OFF);

    lock_init (&tid_lock);
    list_init (&ready_list);
    list_init (&all_list);

    /* Set up a thread structure for the running thread. */
    initial_thread = running_thread ();
    init_thread (initial_thread, "main", PRI_DEFAULT);
    initial_thread->status = THREAD_RUNNING;
    initial_thread->tid = allocate_tid ();
}
```

Pintos provides a certain way to "deactivate" all interrupts, so we first assert
that we are not currently under the effects of said effects. Note that disabling
interrupts basically disables all ways to handle concurrency and other related
capacities.



