#ifndef _cmd_repl_h
#define _cmd_repl_h

#include <devices/input.h>
#include <console.h>
#include "repl.handlers.h"

#define max_cmd 512
#define newroute(cmdname, fnname) {\
    .cmd = cmdname,\
    .fn = fnname,\
}

typedef struct {
    char* str;
    size_t len;
} Line;

typedef struct {
    char* cmd;
    uint8_t (*fn)(void);
} Router;

int strncmp(const char*, const char*, size_t);
size_t getin(Line*, size_t);
uint8_t routecmd(Line*);

int
strncmp(const char* s1, const char* s2, size_t n)
{
    if (n == 0)
        return 0;

    while (n && *s1 && (*s1 == *s2)) {
        ++s1;
        ++s2;
        --n;
    }

    return (*(unsigned char*) s1 - *(unsigned char*) s2);
}

size_t
getin(Line* buffaddr, size_t upto)
{
    printf("$ ");
    buffaddr->len = 0;
    size_t i = 0x00;
    for (; i < upto; i++) {
        uint8_t ch = input_getc();
        if (ch == '\r')
            ch = '\n';

        putchar(ch);
        if (ch == '\n') {
            break;
        }


        buffaddr->str[i] = ch;
        buffaddr->len++;
    }

    return 1;
}

uint8_t
routecmd(Line* cmd)
{
    if (cmd->len == 0) {
        printf("invalid command!\n");
        return 1;
    }

     Router routes[] = {
        newroute("whoami", whoami),
        newroute("ls", ls),
        newroute("exit", exithandler),
    };

    size_t routelen = sizeof(routes) / sizeof(routes[0]);
    for (size_t i = 0x00; i < routelen; i++) {
        if (strncmp(cmd->str, routes[i].cmd, cmd->len) == 0) {
            return routes[i].fn();
        }
    }

    printf("invalid command!\n");
    return 1;
}

#endif

