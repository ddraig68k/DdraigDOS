/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <printf.h>

#include "ddraig.h"
#include "../libraries/fatfs/ff.h"	/* Obtains integer types */

#include "diskio.h"		            /* Declarations of disk functions */
#include "drivers/sys/rtc.h"

DSTATUS g_drive_status[FF_VOLUMES];
FATFS g_fat_fs_workarea[FF_VOLUMES];

ata_identify_info g_drive_id;

//#define DEBUG_PRINT

static const char *g_fatfs_errmsg[20] =
{
    /* 0  */ "Succeeded",
    /* 1  */ "A hard error occurred in the low level disk I/O layer",
    /* 2  */ "Assertion failed",
    /* 3  */ "The physical drive is not operational",
    /* 4  */ "Could not find the file",
    /* 5  */ "Could not find the path",
    /* 6  */ "The path name format is invalid",
    /* 7  */ "Access denied due to prohibited access or directory full",
    /* 8  */ "Access denied due to prohibited access",
    /* 9  */ "The file/directory object is invalid",
    /* 10 */ "The physical drive is write protected",
    /* 11 */ "The logical drive number is invalid",
    /* 12 */ "The volume has no work area",
    /* 13 */ "There is no valid FAT volume",
    /* 14 */ "The f_mkfs() aborted due to any parameter error",
    /* 15 */ "Could not get a grant to access the volume within defined period",
    /* 16 */ "The operation is rejected according to the file sharing policy",
    /* 17 */ "LFN working buffer could not be allocated",
    /* 18 */ "Number of open files > _FS_LOCK",
    /* 19 */ "Given parameter is invalid"
};


int ata_wait_not_busy()
{
    volatile uint32_t timeout = ATA_TIMEOUT;
    do
    {
        uint8_t status = ATA_REG_STATUS;
        if ((status & ATA_ST_BUSY) == 0)
            return 0;
    }
    while (timeout--);

#ifdef DEBUG_PRINT
    printf("ata_wait_not_busy: timeout\n");
#endif
    return 1;
}

int ata_wait_for_data()
{
    volatile uint32_t timeout = ATA_TIMEOUT;
    do
    {
        uint8_t status = ATA_REG_STATUS;
        if ((status & ATA_ST_BUSY) == 0)
            return 0;
    }
    while (timeout--);

#ifdef DEBUG_PRINT
    printf("ata_wait_for_data: timeout\n");
#endif
    return 1;
}

int ata_read_sector(int sector, int count, uint16_t *buffer)
{
	if (ata_wait_not_busy())
        return 1;

	// Read a sector
	ATA_REG_DRIVE_HEAD = 0xE0;

	ATA_REG_SECTOR_NUMBER = (uint8_t) sector;
	ATA_REG_CYLINDER_LOW  = (uint8_t) (sector >> 8);
	ATA_REG_CYLINDER_HIGH = (uint8_t) (sector >> 16);
	ATA_REG_SECTOR_COUNT = count;

	ATA_REG_COMMAND = ATA_CMD_READ_SECTORS;
	if (ata_wait_for_data())
        return 1;


	uint8_t status = ATA_REG_STATUS;
	if (status & ATA_ST_ERROR)
		return 1;

	if (ata_wait_not_busy())
        return 1;

    if (ata_wait_for_data())
    {
        return 1;
    }

	__asm__ volatile
    (
        "move.l	%0, %%a0\n\t"
        "move.l %1, %%a1\n\t"
        "move.l #255, %%d0\n\t"
        ".read_ata: move.w (%%a0),(%%a1)+\n\t"
        "dbra.w %%d0,.read_ata"
        : /* no outputs */
        : "g" (IDE_BASE), "g" (buffer)
        : "%a0", "%a1", "%d0" /* clobbered registers */
	);

	return 0;
}

int ata_write_sector(int sector, int count, uint16_t *buffer)
{
	if (ata_wait_not_busy())
        return 1;

	// Read a sector
	ATA_REG_DRIVE_HEAD = 0xE0;

	ATA_REG_SECTOR_NUMBER = (uint8_t) sector;
	ATA_REG_CYLINDER_LOW  = (uint8_t) (sector >> 8);
	ATA_REG_CYLINDER_HIGH = (uint8_t) (sector >> 16);
	ATA_REG_SECTOR_COUNT = count;

	ATA_REG_COMMAND = ATA_CMD_WRITE_SECTORS;

	uint8_t status = ATA_REG_STATUS;
	if (status & ATA_ST_ERROR)
		return 1;

	if (ata_wait_not_busy())
        return 1;

    if (ata_wait_for_data())
    {
        return 1;
    }

	for (int i = 0; i < 256; i++)
		ATA_REG_DATA = buffer[i];

	return 0;
}

inline uint16_t ata_swap_byte(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

int ata_read_identity(void)
{
    uint16_t *buffer = (uint16_t *)&g_drive_id;

#ifdef DEBUG_PRINT
    printf("ata_read_identity: function entry\n");
#endif

	if (ata_wait_not_busy())
    {
#ifdef DEBUG_PRINT
        printf("ata_read_identity: ata_wait_for_data (1) error\n");
#endif
        return 1;
    }

	ATA_REG_SECTOR_COUNT = 1;
	ATA_REG_COMMAND = ATA_CMD_IDENTIFY;
	if (ata_wait_for_data())
    {
#ifdef DEBUG_PRINT
        printf("ata_read_identity: ata_wait_for_data (1) error\n");
#endif
        return 1;
    }

	uint8_t status = ATA_REG_STATUS;
	if (status & ATA_ST_ERROR)
    {
#ifdef DEBUG_PRINT
        printf("ata_read_identity: disk status error - status = %02X\n", status);
#endif
        return 1;
    }

	if (ata_wait_not_busy())
    {
#ifdef DEBUG_PRINT
        printf("ata_read_identity: ata_wait_not_busy (2) error\n");
#endif
        return 1;
    }

    if (ata_wait_for_data())
    {
#ifdef DEBUG_PRINT
        printf("ata_read_identity: ata_wait_for_data (2) error\n");
#endif
        return 1;
    }

#ifdef DEBUG_PRINT
	printf("ata_read_identity: reading data....");
#endif
    for (int i = 0; i < 256; i++)
    {
		buffer[i] = ata_swap_byte(ATA_REG_DATA);
    }
#ifdef DEBUG_PRINT
    printf("done\n");
#endif
	return 0;
}


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (BYTE pdrv)
{
    if (pdrv >= FF_VOLUMES)
        return STA_NOINIT;

    return g_drive_status[pdrv];
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
    if (pdrv >= FF_VOLUMES)
        return STA_NOINIT;

    g_drive_status[pdrv] = ata_read_identity() == 0 ? 0 : STA_NOINIT;

    return g_drive_status[pdrv];
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv >= FF_VOLUMES)
        return RES_PARERR;

    if (g_drive_status[pdrv] & (STA_NOINIT | STA_NODISK))
        return RES_NOTRDY;

    while (count > 0)
    {
        uint16_t *read_buff = (uint16_t *)buff;
        uint32_t sec_count = 1;
        if (ata_read_sector(sector, sec_count, read_buff))
            return RES_ERROR;

        sector++;
        count--;
        buff += 512;
    }

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
DRESULT disk_write (BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    //printf("DiskIO: drive_write called. pdrv = %d, sector = %d, count = %d\n", (int)pdrv, (int)sector, (int)count);

    if (pdrv >= FF_VOLUMES)
        return RES_PARERR;

    if (g_drive_status[pdrv] & (STA_NOINIT | STA_NODISK))
        return RES_NOTRDY;

    while (count > 0)
    {
        uint16_t *read_buff = (uint16_t *)buff;
        uint32_t sec_count = 1;
        if (ata_write_sector(sector, sec_count, read_buff))
            return RES_ERROR;

        sector++;
        count--;
        buff += 512;
    }

    return RES_OK;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
long get_sector_count(BYTE pdrv)
{
#ifdef DEBUG_PRINT
    printf("DiskIO: get_sector_count called. pdrv = %d\n", (int)pdrv);;
#endif
    return g_drive_id.current_log_sects_per_track;

}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    uint32_t count;
#ifdef DEBUG_PRINT
    printf("DiskIO: disk_ioctrl called. pdrv = %d cmd = %d\n", (int)pdrv, (int)cmd);;
#endif
    if (pdrv >= FF_VOLUMES)
        return RES_PARERR;

    if (g_drive_status[pdrv] & (STA_NOINIT | STA_NODISK))
        return RES_NOTRDY;

    switch (cmd)
    {
        case CTRL_SYNC:
        case CTRL_TRIM:
            return RES_OK;

        case GET_SECTOR_SIZE:
            *((long *)buff) = 512;
            return RES_OK;

        case GET_SECTOR_COUNT:
            count = get_sector_count(pdrv);
            *((uint32_t *)buff) = count;

            if (count == -1)
                return RES_ERROR;

            return RES_OK;

        default:
            return RES_PARERR;
    }
}

void f_perror(int errno)
{
    if (errno <= 19)
        printf("%s\n", g_fatfs_errmsg[errno]);
    else
        printf("Unknown error %d\n", errno);
}

/*-------------------------------------------------------------------*/
/* User Provided RTC Function for FatFs module                       */
/*-------------------------------------------------------------------*/
/* This is a real time clock service to be called from FatFs module. */
/* This function is needed when FF_FS_READONLY == 0 and FF_FS_NORTC == 0 */
DWORD get_fattime(void)
{
    rtc_date_t tm;

    rtc_get_time(&tm);

    DWORD time = (tm.tm_year - 1980) << 25 | tm.tm_mon << 21 | tm.tm_day << 16 | tm.tm_hour << 11 | tm.tm_min << 5 | tm.tm_sec >> 1;
	return time;
}

int initialise_disk(void)
{
    char cmd_buffer[4];

#ifdef DEBUG_PRINT
    if (ata_read_identity())
    {
        printf("initialise_disk: disk identify failed, unable to read disk\n");
        return 1;
    }

    printf("initialise_disk: successfully read disk identity\n");
#endif
    // set up work areas for each volume
    for (int i =0; i < FF_VOLUMES; i++)
    {
    	g_drive_status[i] = STA_NOINIT;

        cmd_buffer[0] = '0' + i;
        cmd_buffer[1] = ':';
        cmd_buffer[2] = 0;

        FRESULT fr = f_mount(&g_fat_fs_workarea[i], cmd_buffer, 0);
        if (fr != FR_OK)
        {
            printf("initialise_disk: Error mounting disk\n");
            f_perror(fr);
        }
    }

#ifdef DEBUG_PRINT
    drive_id.serial_number[19] = 0;
    drive_id.firmware_revision[7] = 0;
    drive_id.model_number[39] = 0;
    printf("initialise_disk: Disk information\n");
    printf("    serial number      : %s\n", drive_id.serial_number);
    printf("    firmwar rev        : %s\n", drive_id.firmware_revision);
    printf("    model number       : %s\n", drive_id.model_number);
    printf("    logical cylinders  : %d\n", drive_id.current_log_cyls);
    printf("    logical heads      : %d\n", drive_id.current_log_heads);
    printf("    sectors per track  : %d\n", drive_id.current_log_sects_per_track);
#endif

    return 0;
}
