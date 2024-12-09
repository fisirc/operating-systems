# Conditional variables


```c
/** Condition variable. */
struct condition
{
    struct list waiters;        /**< List of waiting threads. */
};

void cond_init (struct condition *);
void cond_wait (struct condition *, struct lock *);
void cond_signal (struct condition *, struct lock *);
void cond_broadcast (struct condition *, struct lock *);
```

In order to fully implement the basic priority scheduler, I need to fully
understand what a `conditional variable` is.

Condition variables store 1 single thing: a list of semaphores, this due to the
original rationale behind their existence, which is the "unintuitive" approach
sempahores give to synchronization problems, as exposed in [https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Condvars.html](https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/Condvars.html):

> [...] the practice of incrementing and decrementing an integer value does not
> have an obvious mapping to synchronization problems. For instance, this
> contrasts with the more intuitive lock and unlock operations on mutex locks.

Condvars overcome this by "waiting" and "signaling" (waking up) one or more
threads by the controlling the semaphores they were blocked by. This may sound a
little weird so let's take a look at the code instead:

```c
/** One semaphore in a list. */
struct semaphore_elem
{
    struct list_elem elem;              /**< List element. */
    struct semaphore semaphore;         /**< This semaphore. */
};

void
cond_wait (struct condition *cond, struct lock *lock)
{
    struct semaphore_elem waiter;

    ASSERT (cond != NULL);
    ASSERT (lock != NULL);
    ASSERT (!intr_context ());
    ASSERT (lock_held_by_current_thread (lock));

    sema_init (&waiter.semaphore, 0);
    list_push_back (&cond->waiters, &waiter.elem);
    lock_release (lock);
    sema_down (&waiter.semaphore);
    lock_acquire (lock);
}
```

As you can see, the condvar is releasing some lock and then storing a semaphore
in the `cond::waiters` list, this before blocking the thread.

> A thread must acquire the mutex before calling `pthread_cond_wait()`, which
> will release the mutex. [this is so threads are atomically added to the
> `waiters` queue]

The thread is not ever going to wake up again unless the semaphore that blocked
it goes up again (that is, by using `sema_up()`).

```c
void
cond_signal (struct condition *cond, struct lock *lock UNUSED)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  if (!list_empty (&cond->waiters))
    sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}
```

Et vòila, a previously blocked thread which semaphore was now stored by the
condvar now gets waken up by `sema_up`'ing its semaphore. Repeat this logic in a
loop and you have a "broadcast" that will wake up every thread:

```c
void
cond_broadcast (struct condition *cond, struct lock *lock)
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}
```

