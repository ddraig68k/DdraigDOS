#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <printf.h>

void dump_memory(void *start, int len)
{
    int i, rem;
    unsigned char *ptr=(unsigned char *)start;
    char linebuffer[17], *lbptr;

    for(i = 0; i < 16; i++)
        linebuffer[i] = ' ';

    linebuffer[16]=0;
    lbptr = &linebuffer[0];

    printf("%08X ", (unsigned)ptr&(~15));
    for (i = 0; i < ((unsigned)ptr & 15); i++)
    {
        printf("   ");
        lbptr++;
    }

    while(len)
    {
        if (*ptr >= 32 && *ptr < 127)
            *lbptr = *ptr;
        else
            *lbptr = '.';

        printf(" %02X", *ptr++);
        len--;
        lbptr++;

        if ((unsigned)ptr % 16 == 0)
        {
            printf("  %s", linebuffer);
            lbptr = &linebuffer[0];

            if (len)
                printf("\n%08lX ", (uint32_t)ptr);
            else
            {
                /* no ragged end to tidy up! */
                printf("\n");
                return;
            }
        }
    }

    rem = 16 - ((unsigned)ptr & 15);

    for (i = 0; i < rem; i++)
    {
        printf("   ");
        *lbptr = ' ';
        lbptr++;
    }
    printf("  %s\n", linebuffer);
}
