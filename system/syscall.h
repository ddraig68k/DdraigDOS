#ifndef DDRAIGOS_SYSCALL_H
#define DDRAIGOS_SYSCALL_H

#include <ddraig.h>

typedef struct
{
    uint32_t    command;
	uint32_t	d0, d1, d2;
	void        *a0, *a1, *a2;
} syscall_data;

void init_syscall(void);

// File and disk calls, used by newlib
#define DISK_NOP            	0
#define DISK_FILEOPEN       	1
#define DISK_FILECLOSE      	2
#define DISK_FILEREAD       	3
#define DISK_FILEWRITE      	4
#define DISK_FILESEEK			5
#define DISK_FILESTRUCT			6
#define DISK_FILESTAT			7
#define DISK_FILERENAME			8
#define DISK_FILEDELETE			9

// System and control calls
#define SYS_NOP					0
#define SYS_TIMEDATE			1
#define SYS_TICKCOUNT       	2

#define SYS_INPUT_HAS_CHAR		5		/* Check if there's a character in the input buffer */
#define SYS_INPUT_READ			6		/* Get a character(s) from the input buffer */
#define SYS_OUTPUT_WRITE		7		/* Write data to the standard output */

#define SYS_EXPANSION_COUNT  	10   	/* Check how many expansion boards are installed */
#define SYS_CHECK_EXP_INSTALLED	11		/* Check if device is installed by device ID */
#define SYS_EXPANSION_INFO  	12   	/* Get expansion_dev_t structure for specified slot */

#define SYS_DISPLAY_INFO    	15   	/* Get display_t structure for display */
#define SYS_DISPLAY_DISABLE		16		/* Disable OS handling of the display controller */
#define SYS_DISPLAY_ENABLE		17		/* Enable OS handling of the display controller */

#define SYS_BASIC_HAS_CHAR      20      /* Check for character available for EhBASIC */
#define SYS_BASIC_GET_CHAR      21      /* Get a character for EhBASIC */
#define SYS_BASIC_PUT_CHAR      22      /* Write a character for EhBASIC */

#define SYS_RESET_SYSTEM        50      /* Reset the computer */

#endif
