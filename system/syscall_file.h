#ifndef DDRAIGOS_SYSCALL_FILE_H
#define DDRAIGOS_SYSCALL_FILE_H

#include "syscall.h"

#define FIRST_FILE_NUMBER 3
#define MAX_OPEN_FILES 32

void init_file_syscall(void);

int bios_file_open(syscall_data *sys);
int bios_file_close(syscall_data *sys);
int bios_file_read(syscall_data *sys);
int bios_file_write(syscall_data *sys);
int bios_file_seek(syscall_data *sys);
int bios_file_struct(syscall_data *sys);
int bios_file_stat(syscall_data *sys);
int bios_file_rename(syscall_data *sys);
int bios_file_delete(syscall_data *sys);

#endif
