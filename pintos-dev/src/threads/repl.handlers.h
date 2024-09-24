#ifndef _cmd_handlers_h
#define _cmd_handlers_h

#include <stdio.h>

#define student_code "22200232"

uint8_t whoami(void);
uint8_t ls(void);
uint8_t exithandler(void);

uint8_t
whoami(void)
{
    printf(student_code "\n");
    return 1;
}

uint8_t
ls(void)
{
    printf("tf you expected to have here, webonazo\n");
    return 1;
}

uint8_t
exithandler(void)
{
    printf("goodbye!\n");
    return 0;
}


#endif

