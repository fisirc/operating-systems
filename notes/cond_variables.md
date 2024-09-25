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

> A monitor is a higher-level form of synchronization than a semaphore or a
> lock.
>
> - A monitor consists of data being synchronized, plus a lock, called the
> monitor lock, and one or more condition variables.
>
> - Before it accesses the protected data, a thread first acquires the monitor
> lock. It is then said to be "in the monitor".
>
> - While in the monitor, the thread has control over all the protected data,
> which it may freely examine or modify. When access to the protected data is
> complete, it releases the monitor lock.
>
> - Condition variables allow code in the monitor to wait for a condition to
>   become true. Each condition variable is associated with an abstract
>   condition, e.g. "some data has arrived for processing" or "over 10 seconds
>   has passed since the user's last keystroke".
>
>   - When code in the monitor needs to wait for a condition to become true,
>   it "waits" on the associated condition variable, which releases the lock
>   and waits for the condition to be signaled.
>
>   - If, on the other hand, it has caused one of these conditions to become
>   true, it "signals" the condition to wake up one waiter, or "broadcasts"
>   e condition to wake all of them.


