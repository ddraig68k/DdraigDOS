#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

#include "commands.h"
#include "decodecmd.h"
#include "conio.h"

#include "libraries/fatfs/ff.h"
#include "libraries/srecord.h"
#include "drivers/diskio.h"
#include "drivers/sys/rtc.h"
#include "drivers/sys/duart.h"
#include "system/syscall.h"
#include "system/ostimer.h"

#include "libraries/memdump.h"

extern const cmd_entry_t g_cmd_table[];

static int fromhex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';

    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    return -1;
}

void do_help(int argc, char *argv[])
{
	int i = 0;

	printf("\nAvailable commands:\n");

	while (g_cmd_table[i].name)
    {
		printf("%12s - %s\n", g_cmd_table[i].name, g_cmd_table[i].helpme);
		++i;
	}
}

void do_notimplemented(int argc, char *argv[])
{
    printf("This command is currently not implemented\n");
}


void do_dump(int argc, char *argv[]){
    unsigned long start, count;

    start = strtoul(argv[0], NULL, 16);
    count = strtoul(argv[1], NULL, 16);

    dump_memory((void*)start, count);
}

uint8_t serial_has_char();
uint8_t serial_get();

extern bool g_has_display;

void do_srecord(int argc, char *argv[])
{
    char *rec_buf = (char *)0x00300000;
    unsigned int timeout = 10000;
    bool has_display = g_has_display;

    printf("Download for S-Record file, waiting for serial transfer\n");

    // Disable printing to display as currently the display output is slow
    // This can cause timing issues if the display scrolls
    g_has_display = false;

    // wait for the first character
    *rec_buf++ = serial_get();

    // recieve characters until transmit stops
    while (timeout--)
    {
        if (serial_has_char())
        {
            *rec_buf++ = serial_get();
            timeout = 10000;
            if (((uint32_t)rec_buf % 250) == 0)
                _putchar('.');
        }
    }
    g_has_display = has_display;

    printf("\nData recieved, processing S-Record file\n");
    rec_buf = (char *)0x00300000;
    if (read_srecord(rec_buf))
        printf("SRecord load failed\n");
}

void do_binfile(int argc, char *argv[])
{
    char *rec_buf = (char *)0x00100000;
    char *buffer = (char *)0x00100000;
    unsigned int timeout = 10000;
    unsigned int bytes_transferred = 0;

    if (argc != 1)
    {
        printf("binfile: Filename should be specified\n");
    }

    char *filename = argv[0];
    printf("Download binary file to %s, waiting for serial transfer start\n", filename);

    DISABLE_INTERRUPTS();
    // wait for the first character
    *rec_buf++ = serial_get();
    bytes_transferred++;

    // recieve characters until transmit stops
    while (timeout--)
    {
        if (serial_has_char())
        {
            *rec_buf++ = serial_get();
            bytes_transferred++;
            timeout = 10000;

            if (((uint32_t)rec_buf & 0x000000FF) == 0)
                _putchar_serial('.');
        }
    }
    printf("\nData recieved, saving to file\n");
    ENABLE_INTERRUPTS();

    FIL dest;

    if (f_open(&dest, filename, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK)
    {
        printf("Unable to open destination file %s\n", filename);
        return;
    }

    unsigned int bytes_written = 0;
    if (f_write(&dest, buffer, bytes_transferred, &bytes_written) != FR_OK)
    {
        printf("Unable to write to destination file %s\n\t", filename);
    }
    else
        printf("Written %d bytes to %s\n", bytes_written, filename);

    f_close(&dest);
}

void do_binmem(int argc, char *argv[])
{
    char *rec_buf = (char *)0x00100000;
    unsigned int timeout = 10000;
    unsigned int bytes_transferred = 0;

    if (argc != 1)
    {
        printf("binmem: memory address should be specified\n");
    }

    unsigned long addr;
    addr = strtoul(argv[0], NULL, 16);

    printf("Download binary file to 0x%06x, waiting for serial transfer start\n", addr);

    rec_buf = (char *)addr;

    DISABLE_INTERRUPTS();
    // wait for the first character
    *rec_buf++ = serial_get();
    bytes_transferred++;

    // recieve characters until transmit stops
    while (timeout--)
    {
        if (serial_has_char())
        {
            *rec_buf++ = serial_get();
            bytes_transferred++;
            timeout = 10000;

            if (((uint32_t)rec_buf & 0x000000FF) == 0)
                _putchar_serial('.');
        }
    }
    ENABLE_INTERRUPTS();
    printf("\nWritten %d bytes to memory at 0x%06X\n", bytes_transferred, addr);
}


void do_run(int argc, char *argv[])
{
    unsigned long start;
    start = strtoul(argv[0], NULL, 16);

    void (*entry)(void) = (void (*)(void))start;
    entry();
}

void do_writemem(int argc, char *argv[])
{
    unsigned long value;
    unsigned char *ptr;
    int i, j, l;

    value = strtoul(argv[0], NULL, 16);
    ptr = (unsigned char*)value;

    /* This can deal with values like: 1, 12, 1234, 123456, 12345678.
       Values > 2 characters are interpreted as big-endian words ie
       "12345678" is the same as "12 34 56 78" */

    /* first check we're happy with the arguments */
    for (i = 1; i < argc; i++)
    {
        l = strlen(argv[i]);

        if (l != 1 && l % 2)
        {
            printf("Ambiguous value: \"%s\" (odd length).\n", argv[i]);
            return; /* abort! */
        }

        for (j = 0; j < l; j++)
            if(fromhex(argv[i][j]) < 0)
            {
                printf("Bad hex character \"%c\" in value \"%s\".\n", argv[i][j], argv[i]);
                return; /* abort! */
            }
    }

    /* then we do the write */
    for (i = 1; i < argc; i++)
    {
        l = strlen(argv[i]);
        if (l <= 2) /* one or two characters - a single byte */
            *(ptr++) = strtoul(argv[i], NULL, 16);
        else
        {
            /* it's a multi-byte value */
            j = 0;
            while(j < l)
            {
                value = (fromhex(argv[i][j]) << 4) | fromhex(argv[i][j+1]);
                *(ptr++) = (unsigned char)value;
                j += 2;
            }
        }
    }
}

void do_dir(int argc, char *argv[])
{
    FRESULT fr;
    const char *path, *filename;
    DIR fat_dir;
    FILINFO fat_file;
    uint16_t dir = 1;
    //int i;

    if(argc == 0)
        path = "";
    else
        path = argv[0];

    fr = f_opendir(&fat_dir, path);
    if (fr != FR_OK)
    {
        printf("f_opendir(\"%s\"): ", path);
        f_perror(fr);
        return;
    }

    while (1)
    {
        fr = f_readdir(&fat_dir, &fat_file);
        if (fr != FR_OK)
        {
            printf("f_readdir(): ");
            f_perror(fr);
            break;
        }

        if (fat_file.fname[0] == 0) /* end of directory? */
            break;


        // if ((fat_file.fname[0] & 0xFF) == 0 || (fat_file.fname[0] & 0xFF) == 0xCF) /* end of directory? */
        //     break;

        if (((fat_file.fname[0] & 0xFF) == 0xCF) || (fat_file.altname[0] == 0x00))
            continue;                   // Deleted file?

        filename = fat_file.fname;

        dir = fat_file.fattrib & AM_DIR;

        if (dir)
        {
            /* directory */
            printf("%04d/%02d/%02d %02d:%02d    <DIR> %s/", 1980 + ((fat_file.fdate >> 9) & 0x7F),
                    (fat_file.fdate >> 5) & 0xF, fat_file.fdate & 0x1F,
                    fat_file.ftime >> 11, (fat_file.ftime >> 5) & 0x3F, filename);

            //for (i = strlen(fat_file.fname); i < 12; i++)
            //    printf(" ");
        }
        else if (!(fat_file.fattrib & AM_HID))
        {
            /* regular file */
            printf("%04d/%02d/%02d %02d:%02d %8ld %-12s", 1980 + ((fat_file.fdate >> 9) & 0x7F),
                    (fat_file.fdate >> 5) & 0xF, fat_file.fdate & 0x1F, fat_file.ftime >> 11,
                    (fat_file.ftime >> 5) & 0x3F, fat_file.fsize, filename);
        }

        printf(" \n");
    }

    fr = f_closedir(&fat_dir);
    if (fr != FR_OK)
    {
        printf("f_closedir(): ");
        f_perror(fr);
        return;
    }
}

void do_delete(int argc, char *argv[])
{
    FRESULT fr;
    const char *path;

    path = argv[0];

    fr = f_unlink(path);

    if (fr != FR_OK)
    {
        printf("Error deleting file %s: ", path);
        f_perror(fr);
    }
}

void do_mkdir(int argc, char *argv[])
{
    FRESULT fr;
    const char *path;

    path = argv[0];

    fr = f_mkdir(path);

    if (fr != FR_OK)
    {
        printf("Error creating folder %s: ", path);
        f_perror(fr);
    }
}

void do_rename(int argc, char *argv[])
{
    FRESULT fr;
    const char *srcpath;
    const char *destpath;

    srcpath = argv[0];
    destpath = argv[1];

    fr = f_rename(srcpath, destpath);

    if (fr != FR_OK)
    {
        printf("Error renaming file %s to %s: ", srcpath, destpath);
        f_perror(fr);
    }
}


void do_cd(int argc, char *argv[])
{
    FRESULT r;

    r = f_chdir(argv[0]);
    if(r != FR_OK)
        f_perror(r);
}

void do_loadmem(int argc, char *argv[])
{
    FIL file;
    uint32_t location;
    char *buffer = NULL;

    if (argc == 2)
        location = strtoul(argv[1], NULL, 16);


    if (f_open(&file, argv[0], FA_READ) == FR_OK)
    {
        unsigned int len = f_size(&file);

        if (argc == 2)
            buffer = (char *)location;
        else
        {
            buffer = malloc(len);
            if (buffer == NULL)
            {
                printf("Unable to allocate %d for file.\n", len);
                f_close(&file);
                return;
            }
        }
        unsigned int bytes_read = 0;

        if (f_read(&file, buffer, len, &bytes_read) != FR_OK)
        {
            printf("Unable to load file.\n");
            f_close(&file);
            return;
        }

        printf("Loaded %d bytes from file %s to location %lX\n", bytes_read, argv[0], (uint32_t)buffer);
    }
    else
    {
        printf("Unable to open file %s\n", argv[0]);
    }

    if (argc == 1)
        free(buffer);

    f_close(&file);
}

void do_copyfile(int argc, char *argv[])
{
    FIL src;
    FIL dest;

    char *srcname = argv[0];
    char *destname = argv[1];

    char *buffer = NULL;

    if (f_open(&src, srcname, FA_READ) == FR_OK)
    {
        unsigned int len = f_size(&src);

        buffer = malloc(len);
        if (buffer == NULL)
        {
            printf("Unable to allocate %d for file.\n", len);
            f_close(&src);
            return;
        }

        unsigned int bytes_read = 0;

        if (f_read(&src, buffer, len, &bytes_read) != FR_OK)
        {
            printf("Unable to load source file.\n");
            f_close(&src);
            return;
        }

        if (f_open(&dest, destname, FA_WRITE | FA_CREATE_NEW) != FR_OK)
        {
            printf("Unable to open destination file %s\n", destname);
            f_close(&src);
            free(buffer);
            return;
        }
        unsigned int bytes_written = 0;
        if (f_write(&dest, buffer, len, &bytes_written) != FR_OK)
        {
            printf("Unable to create destination file %s\n\t", destname);
        }
        else
            printf("Copied %d bytes from %s to %s\n", bytes_written, srcname, destname);
    }
    else
    {
        printf("Unable to open source file %s\n", srcname);
        return;
    }

    free(buffer);

    f_close(&src);
    f_close(&dest);
}

void do_cat(int argc, char *argv[])
{
    FIL file;
    char *buffer = NULL;

    if (f_open(&file, argv[0], FA_READ) == FR_OK)
    {
        unsigned int bytes_read = 0;
        unsigned int len = f_size(&file);
        buffer = malloc(len);
        if (buffer == NULL)
        {
            printf("Unable to allocate %d for file.\n", len);
            f_close(&file);
            return;
        }

        if (f_read(&file, buffer, len, &bytes_read) != FR_OK)
        {
            printf("Unable to load file.\n");
            f_close(&file);
            return;
        }

        for (int i = 0; i < len; i++)
            _putchar(buffer[i]);
    }
    else
    {
        printf("Unable to open file %s\n", argv[0]);
    }

    printf("\n");
    free(buffer);
    f_close(&file);
}

void do_time(int argc, char *argv[])
{
    if (argc == 0)
    {
        rtc_print_time();
        return;
    }

    if (argc != 6)
    {
        printf("To set the date and time use: time <year> <mon> <day> <hour> <min> <sec>\n");
        return;
    }

    rtc_date_t date;
    date.tm_year = atoi(argv[0]);
    date.tm_mon  = atoi(argv[1]);
    date.tm_day  = atoi(argv[2]);
    date.tm_hour = atoi(argv[3]);
    date.tm_min  = atoi(argv[4]);
    date.tm_sec  = atoi(argv[5]);

    printf("Setting the date and time to %02d/%02d/%04d %02d:%02d:%02d\n", date.tm_day, date.tm_mon, date.tm_year, date.tm_hour, date.tm_min, date.tm_sec);
    rtc_set_time(&date);
}

void do_uptime(int argc, char *argv[])
{
    uint32_t uptime = get_tick_count();

    uptime /= 10;   // convert to seconds
    uint16_t seconds = (uint16_t)(uptime % 60);
    uptime /= 60;
    uint16_t minutes = (uint16_t)(uptime % 60);
    uptime /= 60;
    uint16_t hours = (uint16_t)(uptime % 24);
    uint16_t days = (uint16_t)(uptime /= 60);

    printf("Uptime: %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
}


void do_traptest(int argc, char *argv[]){
    uint32_t cmd, value;
    char *str;

    syscall_data sys;

    int ret;

    cmd = strtoul(argv[0], NULL, 16);
    value = strtoul(argv[1], NULL, 16);
    str = argv[2];
    sys.command = cmd;
    sys.d0 = value;
    sys.a0 = str;

	__asm__ volatile(
	"move.l	%1, %%a0\n"
	"trap	#15\n"
	"move.l %%d0, %0\n"
	: "=g" (ret)
	: "g" (&sys)
	: "%a0"
	);

    printf("Return value = %ld\n", ret);
    printf("  return data = %s\n", (char *)sys.a1);
}

int ehbasic(void);

uint8_t basic_running = 0;

void do_ehbasic(int argc, char *argv[])
{
    ehbasic();
}
