#include "syscall_file.h"
#include <printf.h>
#include <malloc.h>
#include <memory.h>

#include "../libraries/fatfs/ff.h"

void f_perror(int errno);

FIL *file_handles[MAX_OPEN_FILES];

void init_file_syscall(void)
{
    memset(file_handles, 0, sizeof(FIL) * MAX_OPEN_FILES);
}


int allocate_file_handle(void)
{
    int fileid = -1;
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        if (file_handles[i] == NULL)
        {
            FIL *file = malloc(sizeof(FIL));
            if (file != NULL)
            {
                fileid = i;
                file_handles[i] = file;
                break;
            }
        }
    }

    return fileid;
}

void free_file_handle(int fileid)
{
    fileid = fileid;
    if (fileid < 0)
        return;

    if (fileid < MAX_OPEN_FILES)
    {
        FIL *file = file_handles[fileid];
        if (file != NULL)
        {
            free(file);
            file_handles[fileid] = NULL;
        }
    }
}

int bios_file_open(syscall_data *sys)
{
    debug_printf("bios_file_open: Trying to allocate file for %s\n", sys->a0);
    int fileh = allocate_file_handle();
    if (fileh < 0)
    {
        debug_print("bios_file_open: Too many open files\n");
        sys->d1 = FR_TOO_MANY_OPEN_FILES;
        return -1;
    }

    debug_printf("bios_file_open: Allocated file handle: %d\n", fileh + FIRST_FILE_NUMBER);
    FIL *file = file_handles[fileh];
    BYTE mode = sys->d0;

    FRESULT ret = f_open(file, sys->a0, mode);
    if (ret != FR_OK)
    {
        printf("bios_file_open: Failed to open file %s with mode %02X\n", sys->a0, mode);
        printf("bios_file_open: Error = ");
        f_perror(ret);
        free_file_handle(fileh);
        return -1;
    }

    fileh += FIRST_FILE_NUMBER;

    sys->d1 = ret;
    sys->d0 = fileh;

    debug_printf("bios_file_open: Returning (d0, d1): %d, %d\n", sys->d0, sys->d1);
    return fileh;
}

int bios_file_close(syscall_data *sys)
{
    debug_printf("bios_file_close: Trying to close file allocation for file %d\n", sys->d0);
    if (sys->d0 < FIRST_FILE_NUMBER)
        return FR_OK;

    int fileh = sys->d0 - FIRST_FILE_NUMBER;
    if (fileh >= MAX_OPEN_FILES)
    {
        printf("bios_file_close: File ID too high\n");
        sys->d1 = FR_NO_FILE;
        sys->d0 = 0;
        return 0;
    }

    FIL *file = file_handles[fileh];

    f_close(file);
    free_file_handle(fileh);

    sys->d0 = sys->d1 = FR_OK;
    return FR_OK;
}

int bios_file_read(syscall_data *sys)
{
    int fhandle = sys->d0;
    if (fhandle < FIRST_FILE_NUMBER)
    {
        printf("bios_file_read: Need to implement TTY reading\n");
        sys->d1 = FR_OK;
        sys->d0 = 0;
        return 0;
    }

    fhandle -= FIRST_FILE_NUMBER;
    debug_printf("bios_file_read: Attempting to read %d bytes from file %d\n", sys->d1, sys->d0);
    if ((fhandle >= MAX_OPEN_FILES) || (sys->a0 == NULL))
    {
        debug_print("bios_file_read: File ID too high\n");
        sys->d1 = FR_NO_FILE;
        sys->d0 = 0;
        return 0;
    }

    FIL *file = file_handles[fhandle];
    if (file == NULL)
    {
        debug_printf("bios_file_read: File handle for %d doesn't exist\n", sys->d0);
        sys->d0 = 0;
        sys->d1 = FR_NO_FILE;
        return 0;
    }

    unsigned int bytes_read = 0;
    char *buffer = sys->a0;

    FRESULT res = f_read(file, buffer, sys->d1, &bytes_read);
    sys->d1 = res;

    if (res != FR_OK)
    {
        debug_print("bios_file_read: File read failed\n");
        sys->d0 = 0;
        return 0;
    }

    debug_printf("bios_file_read: Successfully read %d bytes\n", bytes_read);
    sys->d0 = bytes_read;
    return bytes_read;
}

int bios_file_write(syscall_data *sys)
{
    int fhandle = sys->d0;
    debug_printf("bios_file_write: Attempting to write %d bytes to file %d\n", sys->d1, sys->d0);

    if ((sys->d0 >= MAX_OPEN_FILES + FIRST_FILE_NUMBER) || (sys->a0 == NULL))
    {
        printf("bios_file_write: File ID too high\n");
        sys->d1 = FR_NO_FILE;
        sys->d0 = 0;
        return 0;
    }

    if (fhandle < FIRST_FILE_NUMBER)
    {
        // Write out data to the terminal
        char *ptr = (char *)sys->a0;
        for (int i = 0; i < sys->d1; ++i)
        {
            int ch = *ptr++;
            _putchar(ch);
        }

        sys->d0 = sys->d1;
        return sys->d1;
    }

    fhandle -= FIRST_FILE_NUMBER;
    FIL *file = file_handles[fhandle];
    if (file == NULL)
    {
        printf("bios_file_write: File handle for %d doesn't exist\n");
        sys->d0 = 0;
        sys->d1 = FR_NO_FILE;
        return 0;
    }

    unsigned int bytes_written = 0;
    char *buffer = sys->a0;

    FRESULT res = f_write(file, buffer, sys->d1, &bytes_written);
    sys->d1 = res;

    if (res != FR_OK)
    {
        printf("bios_file_write: File write failed\n");
        sys->d0 = 0;
        return 0;
    }

    debug_printf("bios_file_write: Successfully wrote %d bytes\n", bytes_written);
    sys->d0 = bytes_written;
    return bytes_written;
}

#define NL_SEEK_SET	    0	/* Seek from beginning of file.  */
#define NL_SEEK_CUR	    1	/* Seek from current position.  */
#define NL_SEEK_END	    2	/* Seek from end of file.  */

int bios_file_seek(syscall_data *sys)
{
    int fhandle = sys->d0;

    if (fhandle < FIRST_FILE_NUMBER)
    {
        sys->d0 = 0;
        sys->d1 = FR_OK;
        return 0;
    }

    fhandle -= FIRST_FILE_NUMBER;
    debug_printf("bios_file_seek: Attempting to seek file postion on file %d\n", sys->d0);
    if (fhandle >= MAX_OPEN_FILES)
    {
        printf("bios_file_seek: File ID too high\n");
        sys->d1 = FR_NO_FILE;
        sys->d0 = 0;
        return 0;
    }

    FIL *file = file_handles[fhandle];
    if (file == NULL)
    {
        printf("bios_file_seek: File handle for %d doesn't exist\n");
        sys->d0 = 0;
        sys->d1 = FR_NO_FILE;
        return 0;
    }

    FRESULT res;
    int seek = (int)sys->d1;

    if (sys->d2 == NL_SEEK_CUR)
        res = f_lseek(file, f_tell(file) + seek);
    else if (sys->d2 == NL_SEEK_END)
        res = f_lseek(file, f_size(file) + seek);
    else
        res = f_lseek(file, seek);

    sys->d1 = res;
    if (res != FR_OK)
    {
        sys->d0 = 0;
        return -1;
    }

    sys->d0 = f_tell(file);
    return sys->d0;
}

int bios_file_struct(syscall_data *sys)
{
    int fhandle = sys->d0;
    if (fhandle < FIRST_FILE_NUMBER)
    {
        sys->d0 = sys->d1 = FR_OK;
        return 0;
    }

    fhandle -= FIRST_FILE_NUMBER;
    debug_printf("bios_file_struct: Returning FIL sruct for file %d\n", sys->d0);
    if (fhandle >= MAX_OPEN_FILES)
    {
        printf("bios_file_struct: File ID too high\n");
        sys->d1 = FR_NO_FILE;
        sys->d0 = 0;
        return -1;
    }

    sys->a0 = file_handles[fhandle];
    sys->d0 = sys->d1 = FR_OK;
    return 0;

}

int bios_file_stat(syscall_data *sys)
{
    if (sys->a0 == NULL || sys->a1 == NULL)
    {
        if (sys->a0 == NULL)
            printf("bios_file_stat: Filename is NULL\n");
        if (sys->a1 == NULL)
            printf("bios_file_stat: Stat buffer is NULL\n");
        sys->d1 = FR_NO_FILE;
        return -1;
    }
    debug_printf("bios_file_stat: Getting f_stat for file %s\n", sys->a0);
    FRESULT res = f_stat((const TCHAR *)sys->a0, (FILINFO *)sys->a1);
    printf("bios_file_stat: f_stat call returned code %d\n", res);
    sys->d0 = sys->d1 = res;
    if (res != FR_OK)
        return -1;

    return 0;
}

int bios_file_rename(syscall_data *sys)
{
    printf("bios_file_rename: Renaming file %s to %s\n", sys->a0, sys->a1);
    if (sys->a0 == NULL || sys->a1 == NULL)
    {
        sys->d1 = FR_NO_FILE;
        return -1;
    }

    FRESULT res = f_rename((const TCHAR *)sys->a0, (const TCHAR *)sys->a1);
    debug_printf("bios_file_rename: f_rename call returned code %d\n", res);
    sys->d1 = res;
    if (res != FR_OK)
        return -1;

    return 0;
}

int bios_file_delete(syscall_data *sys)
{
    debug_printf("bios_file_delete: Deleting file %s\n", sys->a0);
    if (sys->a0 == NULL)
    {
        sys->d1 = FR_NO_FILE;
        return -1;
    }

    FRESULT res = f_unlink((const TCHAR *)sys->a0);
    debug_printf("bios_file_delete: f_unlink call returned code %d\n", res);
    sys->d1 = res;
    if (res != FR_OK)
        return -1;

    return 0;
}
