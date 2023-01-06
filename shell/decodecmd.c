#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <printf.h>

#include "decodecmd.h"
#include "commands.h"
#include "elf.h"
#include "conio.h"

#include "libraries/fatfs/ff.h"
#include "drivers/diskio.h"
#include "drivers/display.h"
#include "drivers/sys/keycodes.h"
#include "drivers/sys/duart.h"

#define MAXARGS 40

void f_perror(int errno);

#define HEADER_EXAMINE_SIZE 4 /* number of bytes we need to load to determine the file type */
const uint8_t g_elf_header_bytes[4]  = { 0x7F, 0x45, 0x4c, 0x46 };

const cmd_entry_t g_cmd_table[] = {
    // name         min max function
    {"basic",       0,  1, &do_ehbasic,     "Start EhBASIC" },
    {"binfile",     1,  1, &do_binfile,     "Recieve binary file over serial and save to disk <filename>" },
    {"binmem",      1,  1, &do_binmem,      "Recieve binary file over serial to location in RAM" },
    {"cat",         1,  1, &do_cat,         "Display contents of <file>"},
    {"cd",          1,  1, &do_cd,          "Change directory <dir>"},
    {"copy",        2,  2, &do_copyfile,    "Copy file <src> <dest>"},
    {"dir",         0,  1, &do_dir,	        "List directory contents" },
    {"del",         1,  1, &do_delete,	    "Delete file <file>" },
    {"dump",        2,  2, &do_dump,        "Dump memory <from> <count>" },
    {"help",	    0,  0, &do_help,	    "Show available commands" },
    {"loadmem",	    1,  2, &do_loadmem,	    "Load a file to the specified memory location <file> <mem>" },
    {"mkdir",	    1,  1, &do_mkdir,	    "Create the specified folder <dir>" },
    {"rename",      2,  2, &do_rename,      "Rename file <src> to <dest>" },
    {"run",         1,  1, &do_run,         "Run code at address <addr>" },
    {"srec",        0,  0, &do_srecord,     "Recieve S-Record file over serial" },
    {"time",        0,  6, &do_time,        "Display or set the current time and date (time ? for help)" },
    {"trap",        3,  3, &do_traptest,    "Test the trap function <command> <value> <string>" },
    {"uptime",      0,  0, &do_uptime,      "Display the time the system has been running" },
    {"writemem",    2,  0, &do_writemem,    "Write memory <addr> [byte ...]" },

    {0, 0, 0, 0, 0 }
};

#define NUM_FILE_EXTENSIONS     3
static const char *g_exe_extensions[NUM_FILE_EXTENSIONS] = {".exe", ".elf", ".bat" };


int getline(char *line, int linesize)
{
	static int count = 0;
	int ret = -1;

    if (has_key() == false)
        return ret;

    if (count < linesize - 1)
    {
        uint8_t ch = get_key();
        switch(ch)
        {
            case PS2_TAB: break;
            case PS2_ENTER:
                _putchar('\n');
                break;

            case PS2_ESC: break;
            case PS2_INSERT: break;

            case PS2_DELETE:
                if (count > 0)
                {
                    _putchar(PS2_DELETE);
                    _putchar_serial('\b');
                    _putchar_serial(' ');
                    _putchar_serial('\b');
                    --count;
                }
                break;

            case PS2_HOME: break;
            case PS2_END: break;
            case PS2_PAGEUP: break;
            case PS2_PAGEDOWN: break;
            case PS2_UPARROW: break;
            case PS2_LEFTARROW: break;
            case PS2_DOWNARROW: break;
            case PS2_RIGHTARROW: break;
            case PS2_F1: break;
            case PS2_F2: break;
            case PS2_F3: break;
            case PS2_F4: break;
            case PS2_F5: break;
            case PS2_F6: break;
            case PS2_F7: break;
            case PS2_F8: break;
            case PS2_F9: break;
            case PS2_F10: break;
            case PS2_F11: break;
            case PS2_F12: break;
            case PS2_SCROLL: break;
            default:
                if (ch >= ' ')
                {
                    line[count++] = ch;
                    _putchar(ch);
                }
                break;
        }

        if (ch == PS2_ENTER)
        {
			line[count] = 0;
			ret = count;
			count = 0;
        }
	}
    else
    {
			line[count] = 0;
			ret = count;
			count = 0;
    }

	return ret;
}

int process_command_builtin(int argc, char *argv[])
{
    const cmd_entry_t *cmd;
    FRESULT fr;

    if (argc == 1 && argv[0][1] == ':')
    {
        // Change current drive
        fr = f_chdrive(argv[0]);
        if (fr)
        {
        	f_perror(fr);
        	printf("Drive %s not found\n", argv[0]);
        }
        return 1;
    }
    else
    {
        // built-in commands
        for (cmd = g_cmd_table; cmd->name; cmd++)
        {
            if (!strcasecmp(argv[0], cmd->name))
            {
                if ((argc - 1) >= cmd->min_args && (cmd->max_args == 0 || (argc - 1) <= cmd->max_args))
                {
                    cmd->function(argc - 1, argv + 1);
                }
                else
                {
                    if (cmd->min_args == cmd->max_args)
                        printf("%s: requires %d argument%s\n", argv[0], cmd->min_args, cmd->min_args == 1 ? "" : "s");
                    else
                        printf("%s: requires %d to %d arguments\n", argv[0], cmd->min_args, cmd->max_args);
                }
                return 1;
            }
        }
    }
    return 0;
}

int load_elf_executable(int argc, char *argv[], FIL *fd)
{
    unsigned int bytesRead;
    uint32_t memHigh = 0;
    uint32_t memLow = 0xFFFFFFFF;
    uint32_t progIndex = 0;
    elf32_header header;
    elf32_program_header progHeader;

    f_lseek(fd, 0);
    if (f_read(fd, &header, sizeof(header), &bytesRead) != FR_OK || bytesRead != sizeof(header))
    {
        printf("Cannot read ELF file header\n");
        return -1;
    }
    else
    {
        debug_printf("ELF file header read, %d bytes\n", bytesRead);
    }

    if (header.ident_magic[0] != 0x7F ||
        header.ident_magic[1] != 'E' ||
        header.ident_magic[2] != 'L' ||
        header.ident_magic[3] != 'F' ||
        header.ident_version != 1)
    {
        printf("Bad ELF header\n");
        return -1;
    }

    if (header.ident_class != ID_32BIT || header.ident_data != ID_BIG_ENDIAN ||
        header.ident_osabi != 0 || header.ident_abiversion != 0)
    {
        printf("Not a 32-bit ELF file.\n");
        return -1;
    }

    if (header.type != ET_EXEC)
    {
        printf("ELF file is not an executable.\n");
        return -1;
    }

    if (header.machine != EM_68K)
    {
        printf("ELF file is not for 68000 processor.\n");
        return -1;
    }

    while (progIndex < header.phnum)
    {
        f_lseek(fd, progIndex * header.phentsize + header.phoff);
        if (f_read(fd, &progHeader, sizeof(progHeader), &bytesRead) != FR_OK || bytesRead != sizeof(progHeader))
        {
            printf("Cannot read ELF program header.\n");
            return -1;
        }

        switch (progHeader.type)
        {
            case PT_NULL:
            case PT_NOTE:
            case PT_PHDR:
                break;

            case PT_SHLIB:
            case PT_DYNAMIC:
                printf("ELF executable is dynamically linked.\n");
                return -1;

            case PT_LOAD:
				debug_printf("Loading %d byte segment from offset 0x%x to address 0x%x\n\r",
					progHeader.filesz, progHeader.offset, progHeader.paddr);
                f_lseek(fd, progHeader.offset);

                if(f_read(fd, (char*)progHeader.paddr, progHeader.filesz, &bytesRead) != FR_OK || bytesRead != progHeader.filesz)
                {
                    printf("Unable to read segment from ELF file.\n");
                    return -1;
                }

                if (progHeader.memsz > progHeader.filesz)
                {
				    debug_printf("Clearing %d bytes BSS at 0x%x\n\r",
					    progHeader.memsz - progHeader.filesz, progHeader.paddr + progHeader.filesz);
                    memset((char*)progHeader.paddr + progHeader.filesz, 0, progHeader.memsz - progHeader.filesz);
                }

                if (progHeader.paddr < memLow)
                    memLow = progHeader.paddr;

                if (progHeader.paddr + progHeader.filesz > memHigh)
                    memHigh = progHeader.paddr + progHeader.filesz;
                break;

            case PT_INTERP:
                printf("ELF executable requires an interpreter.\n");
                return -1;
        }
        progIndex++;
    }

    debug_printf("Program entry point is at 0x%x\n", header.entry);
    debug_printf("Calling with %d args\n", argc);
    for (int i = 0; i < argc; i++)
        debug_printf("  arg %d : %s\n", i, argv[i]);

    debug_printf("Running program %s\n\n", argv[0]);

    int (*entry)(int, char**) = (int (*)(int, char**))header.entry;
    int ret = (*entry)(argc, argv);
    debug_printf("Program returned value %d\n", ret);
    return ret;
}

FRESULT open_executable_file(FIL *file, char *filename)
{
    char fileext[FF_MAX_LFN];
    FRESULT fr;

    // Try opening the file as named
    fr = f_open(file, filename, FA_READ);
    if (fr == FR_OK) /* file doesn't exist? */
        return FR_OK;

    debug_printf("Unable to open file %s, trying with extensions..\n", filename);
    for (int i = 0; i < NUM_FILE_EXTENSIONS; i++)
    {
        debug_printf("Trying extension %s\n", g_exe_extensions[i]);
        if (strlen(filename) + strlen(g_exe_extensions[i]) < FF_MAX_LFN)
        {
            strcpy(fileext, filename);
            strcat(fileext, g_exe_extensions[i]);
            // Try opening the file with extension
            debug_printf("Trying to open file %s ...\n", fileext);
            fr = f_open(file, fileext, FA_READ);
            if (fr == FR_OK) /* file doesn't exist? */
                return FR_OK;
        }
    }

    debug_print("Unable to open file with known extension\n");
    return FR_NO_FILE;
}

int process_command_executable(int argc, char *argv[])
{
    FIL file;
    FRESULT fr;
    char buffer[HEADER_EXAMINE_SIZE];
    unsigned int br;

    fr = open_executable_file(&file, argv[0]);
    if (fr != FR_OK)
    {
        debug_printf("%s: Cannot load: ", argv[0]);
        //f_perror(fr);
        return 0;
    }

    memset(buffer, 0, HEADER_EXAMINE_SIZE);

    debug_printf("%s: File size is %d bytes, ", argv[0], f_size(&file));

    /* sniff the first few bytes, then rewind to the start of the file */
    fr = f_read(&file, buffer, HEADER_EXAMINE_SIZE, &br);
    f_lseek(&file, 0);

    if (fr == FR_OK)
    {
        if (memcmp(buffer, g_elf_header_bytes, sizeof(g_elf_header_bytes)) == 0)
        {
            debug_print("ELF executable\n\n");
            load_elf_executable(argc, argv, &file);
        }
        else
        {
            debug_print("unknown format.\n\n");
            printf("%s: is not an executable file\n", argv[0]);
        }
    }
    else
    {
        printf("%s: Cannot read: ", argv[0]);
        f_perror(fr);
    }

    f_close(&file);

    return 1;
}


void process_command(char *argv[], int argc)
{
    if (argc > 0)
    {
        if (!process_command_builtin(argc, argv) && !process_command_executable(argc, argv))
            printf("%s: Unknown command.  Try 'help'.\n", argv[0]);
    }
}

void execute_cmd(char *linebuffer)
{
    char *args[MAXARGS+1];
    char *ptr;
    int numargs;

    numargs = 0;
    ptr = linebuffer;

    while (1)
    {
        if (numargs == MAXARGS)
        {
            printf("Limiting to %d arguments.\n", numargs);
            *ptr = 0;
        }

        if (*ptr == 0)
        {
            // end of string
            args[numargs] = 0;
            break;
        }

        while (isspace(*ptr))
            ptr++;

        if (!isspace(*ptr))
        {
            args[numargs++] = ptr;

            while (*ptr && !isspace(*ptr))
                ptr++;

            if (*ptr == 0)
                continue;

            while (isspace(*ptr))
            {
                *ptr=0;
                ptr++;
            }
        }
    }

    process_command(args, numargs);
}
