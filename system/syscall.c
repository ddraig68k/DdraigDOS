#include "syscall_file.h"
#include "syscall_sys.h"

void init_syscall(void)
{
    init_file_syscall();
}

int bios_disk(syscall_data *sys)
{
    debug_printf("bios_disk called: command = %ld\n", sys->command);
    switch (sys->command)
    {
        case DISK_FILEOPEN:
            return bios_file_open(sys);

        case DISK_FILECLOSE:
            return bios_file_close(sys);

        case DISK_FILEREAD:
            return bios_file_read(sys);

        case DISK_FILEWRITE:
            return bios_file_write(sys);

        case DISK_FILESEEK:
            return bios_file_seek(sys);

        case DISK_FILESTRUCT:
            return bios_file_struct(sys);

        case DISK_FILESTAT:
            return bios_file_stat(sys);

        case DISK_FILERENAME:
            return bios_file_rename(sys);

        case DISK_FILEDELETE:
            return bios_file_delete(sys);

        default:
            sys->d0 = sys->d1 = 0;
            break;
    }

    return 0;
}

int bios_system(syscall_data *sys)
{
    debug_printf("bios_system called: command = %ld\n", sys->command);
    switch (sys->command)
    {
        case SYS_TIMEDATE:
            return bios_sys_timedate(sys);

        case SYS_TICKCOUNT:
            return bios_sys_tickcount(sys);

        case SYS_INPUT_HAS_CHAR:
            return bios_sys_input_has_char(sys);

        case SYS_INPUT_READ:
            return bios_sys_input_read(sys);

        case SYS_OUTPUT_WRITE:
            return bios_sys_output_write(sys);

        case SYS_EXPANSION_COUNT:
            return bios_sys_get_exp_count(sys);

        case SYS_CHECK_EXP_INSTALLED:
            return bios_sys_check_exp_id(sys);

        case SYS_EXPANSION_INFO:
            return bios_sys_expansion_info(sys);

        case SYS_DISPLAY_INFO:
            return bios_sys_display_info(sys);

        case SYS_DISPLAY_DISABLE:
            return bios_sys_disable_display(sys);

        case SYS_DISPLAY_ENABLE:
            return bios_sys_enable_display(sys);

        case SYS_RESET_SYSTEM:
            return bios_sys_reset(sys);

        case SYS_BASIC_HAS_CHAR:
            return bios_sys_basic_has_char(sys);

        case SYS_BASIC_GET_CHAR:
            return bios_sys_basic_get_char(sys);

        case SYS_BASIC_PUT_CHAR:
            return bios_sys_basic_put_char(sys);

        default:
            sys->d0 = sys->d1 = 0;
            break;
    }

    return 0;
}
