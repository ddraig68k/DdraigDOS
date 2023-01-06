#ifndef DDRAIGOS_SYSCALL_SYS_H
#define DDRAIGOS_SYSCALL_SYS_H

#include "syscall.h"

// System functions
int bios_sys_timedate(syscall_data *sys);
int bios_sys_tickcount(syscall_data *sys);

// Character input/output
int bios_sys_input_has_char(syscall_data *sys);
int bios_sys_input_read(syscall_data *sys);
int bios_sys_output_write(syscall_data *sys);

int bios_sys_get_exp_count(syscall_data *sys);
int bios_sys_check_exp_id(syscall_data *sys);
int bios_sys_expansion_info(syscall_data *sys);

int bios_sys_display_info(syscall_data *sys);
int bios_sys_disable_display(syscall_data *sys);
int bios_sys_enable_display(syscall_data *sys);

int bios_sys_basic_has_char(syscall_data *sys);
int bios_sys_basic_get_char(syscall_data *sys);
int bios_sys_basic_put_char(syscall_data *sys);

int bios_sys_reset(syscall_data *sys);

#endif
