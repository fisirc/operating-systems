# Multi Level Feedback Queue Scheduler

<div style="text-align: center;">
$\text{In computer science, a multilevel feedback queue is a scheduling algorithm.}$

$\text{\it -- what else could it possibly be, smart ass?}$
</div>

## The requirements

There are certain implementation requirements for the MLFQS, these are:

1. Separate processes into various "ready" queues based on their need for the
   processor (hence the "multi-level" part).

1. Give preference to processes with short CPU bursts.

1. Give preference to processes with high I/O bursts.

> a CPU burst is a period during which a process or program demands and actively
> utilizes the Central Processing Unit (CPU) for computation
>
> [*these guys*](https://www.baeldung.com/cs/cpu-io-burst-cycles)



