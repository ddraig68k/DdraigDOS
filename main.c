#include <stdio.h>
#include <stdlib.h>
#include <printf.h>
#include <malloc.h>
#include <string.h>

#include "libraries/fatfs/ff.h"
#include "system/ostimer.h"
#include "system/syscall_file.h"
#include "shell/decodecmd.h"
#include "shell/conio.h"
#include "drivers/diskio.h"
#include "drivers/sys/rtc.h"
#include "drivers/sys/duart.h"
#include "drivers/expansion.h"
#include "drivers/display.h"
#include "drivers/keyboard.h"

#define LINELEN 256
#define DOS_VERSION "0.3.6"

uint32_t g_timer_ticks = 0;
char g_cmd_buffer[LINELEN];

void display_prompt(void)
{
    char path[FF_MAX_LFN];

    f_getcwd(path, FF_MAX_LFN);
    if ('0' <= path[0] && path[0] <= '9')
        path[0] += 'A'-'0';
    printf("%s>", path);
}

int main(void)
{
    // Use non OS SRAM for ROM malloc code
    malloc_init((const void *)HEAP_START, (const void *)(SRAM_END - STACK_SIZE), 256, 16, 4);

    // Ensure keyboard interrupts are disabled
    clear_keyboard_config(0);

    rtc_init();
    initialise_disk();
    init_syscall();
    init_timer();
    ENABLE_INTERRUPTS();

    // Initialise video card if present on bus
    init_expansion_bus();
    uint8_t num_boards = num_exp_boards_installed();

    printf("\nY Ddraig.\nA Motorola 68000 based computer\n\nDdraigDOS version %s\nBuilt on %s %s\n\n", DOS_VERSION, __DATE__, __TIME__);

    printf("\nChecking for keyboard: ");
    DISABLE_INTERRUPTS();
    bool keyboard_present = init_keyboard();

    if (keyboard_present)
    {
        cpu_delay(20000);
        keyboard_flush();
    }

    ENABLE_INTERRUPTS();
    if (keyboard_present)
    {
        enable_keyboard_interrupt();
        printf("PS/2 keyboard detected\n");
    }
    else
        printf("No keyboard detected\n");

    printf("Checking for installed boards: ");
    if (num_boards == 0)
        printf("No boards installed\n");
    else
    {
        if (num_boards == 1)
            printf("%d board installed\n", num_boards);
        else
            printf("%d boards installed\n", num_boards);

        for (int i = 0; i < MAX_EXP_DEVICES; i++)
        {
            if (is_exp_board_present(i))
                printf("    Slot %d: %s\n", i+1, get_expid_description(i));
        }
    }
    rtc_print_time();
    printf("\n");

    // set the initial drive letter
    strcpy(g_cmd_buffer, "0:");
    execute_cmd(g_cmd_buffer);

    display_prompt();
    enable_cursor();

    while (1)
    {
        if (getline(g_cmd_buffer, LINELEN) != -1)
        {
            disable_cursor();
            execute_cmd(g_cmd_buffer);

            display_prompt();
            enable_cursor();
        }

        update_timers();
    }

	return 0;
}

