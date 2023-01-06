#include <stdio.h>
#include <stdint.h>
#include <printf.h>

#define SREC_LINE_LEN   256

char *g_srec_buffer = NULL;
char *g_srec_ptr = NULL;
uint8_t g_srec_checksum;

int srec_getnibble(void)
{
    char ch;

    ch = -1;
    if (g_srec_ptr)
    {
        ch = *g_srec_ptr++;
        if (ch >= '0' && ch <= '9')
            ch -= '0';
        else if (ch >= 'A' && ch <= 'F')
            ch -= 'A' - 10;
        else if (ch >= 'a' && ch <= 'f')
            ch -= 'a'-10;
        else
        {
            printf("Invalid hex digit found\n");
            ch = -1;
        }
   }
   else
        printf("Buffer pointer is null\n");

   return (int)ch;
}

int srec_getbyte(void)
{
   int b = srec_getnibble();
   b <<= 4;
   b += srec_getnibble();
   g_srec_checksum += b;
   return b;
}

int srec_getword(void)
{
   int w = srec_getbyte();
   w <<= 8;
   w += srec_getbyte();
   return w;
}

uint32_t srec_getdword(void)
{
   uint32_t d = srec_getword();
   d <<= 16;
   d += srec_getword();
   return d;
}

/* added for SREC files */
uint32_t srec_get6word(void)
{
   uint32_t d = srec_getword();
   d <<= 8;
   d += srec_getbyte();
   return d;
}

void srec_putbyte(uint32_t addr, uint8_t data)
{
    uint8_t *addrptr = (uint8_t *)addr;
    *addrptr = data;
}

int srec_getline(char *line, int linesize)
{
	int idx = 0;
	char ch;

	do
    {
		ch = *g_srec_buffer++;

		if (ch >= ' ' && ch < 0x7F)
        {
			line[idx++] = ch;
		}
		else if (ch == '\r' || ch == '\n')
        {
			ch = 0;
		}
	}
    while (ch && idx < linesize - 1);

	line[idx] = 0;

	return idx;
}


int read_srecord(char *data_buffer)
{
    uint8_t rectype;
    uint8_t data_len;
    uint8_t data;
    uint32_t address = 0;
    int     lineno = 0;
    uint8_t  srec_end = 0;

    int index;
    g_srec_buffer = data_buffer;

    char srec_line[SREC_LINE_LEN];

    do
    {
        if (srec_getline(srec_line, SREC_LINE_LEN) == 0)
            continue;

        if ((uint32_t)g_srec_buffer - (uint32_t)data_buffer > 0x100000)
        {
            printf("S-Record buffer exceeded max len, aborting\n");
            return 1;
        }


        g_srec_ptr = srec_line;
        lineno++;

        if (*g_srec_ptr++ != 'S')
         {
            printf("Incorrect Line format: %s\n", --g_srec_ptr);
            return 1;
        }

        g_srec_checksum = 0;
        rectype = srec_getnibble();
        data_len = srec_getbyte();

        //printf("SRec: rectype=%02d datalen=%02d : %s\n", rectype, data_len, srec_line);

		switch (rectype)
        {
			case 0:
			case 1:
			case 5:
			case 9:
				address = srec_getword();
				data_len -= 2;
				break;

			case 2:
			case 8:
				address = srec_get6word();
				data_len -= 3;
				break;

			case 3:
			case 7:
				address = srec_getdword();
				data_len -= 4;
				break;

            default:
                printf("S-Record: Unknown record type: %d\n", rectype);
		}

		if (rectype >= 1 && rectype <= 3)
        {
            index = 0;
            while (--data_len)
            {
                data = srec_getbyte();
                srec_putbyte(address + index, data);
                index++;
            }
		}
		else if (rectype == 0)
        {
			printf("Comment: ");
			while (--data_len)
            {
				printf("%c", (char)srec_getbyte());
			}
			printf("\n");
		}
		else if (rectype == 8 || rectype == 9)
        {
			printf("End of S-Record found\n");
            srec_end = 1;
		}
	    else
        {
            // records 5,7 are ignored
            printf("Record type %d is being ignored\n", (int)rectype);
        }

        data = srec_getbyte();           // get final checksum
        if (g_srec_checksum != 0xFF)
        {
            printf("S-Record: Checksum failure\n");
        }
    }
    while (!srec_end);

    printf("S-Record loaded: %d lines\n", lineno);

    return 0;
}

