#include "helpers.h"
#include <stdio.h>
#include <string.h>

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

int get_line(char *prmpt, char *buff, size_t sz)
{
    int ch, extra;

    // Get line with buffer overrun protection.
    if (prmpt != NULL)
    {
        printf("%s", prmpt);
        fflush(stdout);
    }
    if (fgets(buff, sz, stdin) == NULL)
        return NO_INPUT;

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff) - 1] != '\n')
    {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? TOO_LONG : OK;
    }

    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff) - 1] = '\0';
    return OK;
}

void clearBuffer(char *buf, int charlen)
{
    int i;
    for (i = 0; i < charlen; i++)
        buf[i] = '\0';
}

void print_intro()
{
    printf("\n\n\n"
    " ______     _                   _____ _     _    _ ____    \n"
    "|  ____|   | |                 / ____| |   | |  | |  _ \\  \n"
    "| |__ _   _| |_ _   _ _ __ ___| |    | |   | |  | | |_) | \n"
    "|  __| | | | __| | | | '__/ _ \\ |    | |   | |  | |  _ <  \n"
    "| |  | |_| | |_| |_| | | |  __/ |____| |___| |__| | |_) | \n"
    "|_|   \\__,_|\\__|\\__,_|_|  \\___|\\_____|______\\____/|____/  \n\n\n");
}