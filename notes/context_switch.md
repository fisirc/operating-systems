# The Context Switcher

Before actually diving into the code, there are some architecture details that
we must know. PintOS is designed to work under a x86 arch of 32 bits and the
IA-32 instruction set, under this fact we can be totally sure that an `int` type
is of 32 bits, just as a `uintptr_t` one.

The principal function defined at `switch.S` is called `switch_threads`:

```asm
.globl switch_threads
.func switch_threads
switch_threads:
    # Save caller's register state.
    #
    # Note that the SVR4 ABI allows us to destroy %eax, %ecx, %edx,
    # but requires us to preserve %ebx, %ebp, %esi, %edi.  See
    # [SysV-ABI-386] pages 3-11 and 3-12 for details.
    #
    # This stack frame must match the one set up by thread_create()
    # in size.
    pushl %ebx
    pushl %ebp
    pushl %esi
    pushl %edi
```

The .globl tag signs a certain symbol to be visible by the linker (`ld`) and in
this case the symbol is a function `.func switch_threads`. At `switch.h` we
define the function and its parameters like:

```asm
struct thread *switch_threads (struct thread *cur, struct thread *next);
```

As specified in the i386 (x86)
[calling convention p43 et p39](https://refspecs.linuxfoundation.org/elf/abi386-4.pdf),
all arguments are passed through the stack, so in this case, and after pushing
our need-to-preserve registers, the stack ends up as follows:

```
+---------------------------+---------------+
|   name                    |   offset      |
+---------------------------+---------------+
|   struct thread * next    |   24(%esp)    | # p37 specifies arg order in %esp
|   struct thread * cur     |   20(%esp)    |
|   void* eip               |   16(%esp)    | # p39 specifies this %eip behavior
|   %ebx                    |   12(%esp)    |
|   %ebp                    |    8(%esp)    |
|   %esi                    |    4(%esp)    |
|   %edi                    |    0(%esp)    |
+---------------------------+---------------+
```

Theres another symbol here (thread\_stack\_ofs), but, since we are never actually
defining it, ld will mark it as "U" and search for its definition at
linking-time.

```asm
    ...
    pushl %edi

    # Get offsetof (struct thread, stack).
.globl thread_stack_ofs
    mov thread_stack_ofs, %edx

    # Save current stack pointer to old thread's stack, if any.
    movl SWITCH_CUR(%esp), %eax
    movl %esp, (%eax,%edx,1)

    # Restore stack pointer from new thread's stack.
    movl SWITCH_NEXT(%esp), %ecx
    movl (%ecx,%edx,1), %esp

    # Restore caller's register state.
    popl %edi
    popl %esi
    popl %ebp
    popl %ebx
        ret
.endfunc
```

`thread_stack_ofs` marks the absolute address offset from some `struct thread *`
type to its `stack` pointer field.

Now, we are basically assigning the `esp` to
`stack_ofs(current_thread_pointer)`, this means that now `struct thread * cur`
will store the og stack address with the og important registers.

After that, we do the same but in reverse, we will take the stack pointer from
the new `struct thread * next` thread and "repop" the important registers from
there.

After that, the `ret` instruction will jump to the new thread's stack's eip.
Effectively changing the current thread's current executing functing.

