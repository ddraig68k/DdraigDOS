
#ifndef DDRAIG_OS_DISKIO_H
#define DDRAIG_OS_DISKIO_H

#include "ddraig.h"

#define	ATA_REG_DATA	        *((volatile uint16_t *) (IDE_BASE + 0x00))            /* Data Register (R/W) */
#define	ATA_REG_DATA_BYTE       *((volatile uint8_t  *) (IDE_BASE + 0x00))            /* Data Register (R/W) */
#define	ATA_REG_ERROR   	    *((volatile uint8_t  *) (IDE_BASE + 0x02))            /* Error Register (R) */
#define	ATA_REG_PRECOMP	        *((volatile uint8_t  *) (IDE_BASE + 0x02))            /* Write Precomp register (W) */
#define	ATA_REG_SECTOR_COUNT	*((volatile uint8_t  *) (IDE_BASE + 0x04))            /* Sector Count (R/W) */
#define	ATA_REG_SECTOR_NUMBER   *((volatile uint8_t  *) (IDE_BASE + 0x06))            /* Sector Number (LBA bits 0-7) (R/W) */
#define	ATA_REG_CYLINDER_LOW    *((volatile uint8_t  *) (IDE_BASE + 0x08))            /* Cylinder Low (LBA bits 8-15) (R/W) */
#define	ATA_REG_CYLINDER_HIGH   *((volatile uint8_t  *) (IDE_BASE + 0x0A))            /* Cylinder High (LBA bits 16-23) (R/W) */
#define	ATA_REG_DRIVE_HEAD      *((volatile uint8_t  *) (IDE_BASE + 0x0C))            /* Device/Head Register (R/W) */
#define	ATA_REG_STATUS	        *((volatile uint8_t  *) (IDE_BASE + 0x0E))            /* Status Register (R) */
#define	ATA_REG_COMMAND	        *((volatile uint8_t  *) (IDE_BASE + 0x0E))            /* Command Register (W) */
#define ATA_REG_ALT_STATUS      *((volatile uint8_t  *) (IDE_BASE + 0x8C))            /* Alternate Status (R) */
#define ATA_REG_DRIVE_CONTROL   *((volatile uint8_t  *) (IDE_BASE + 0x8C))            /* Interrupt/Reset Register (W) */
#define ATA_REG_DRIVE_ADDR      *((volatile uint8_t  *) (IDE_BASE + 0x8E))            /* Drive Address (R) */

#define ATA_CMD_READ_SECTORS	0x20
#define ATA_CMD_WRITE_SECTORS	0x30
#define ATA_CMD_IDENTIFY	    0xEC
#define ATA_CMD_SET_FEATURE	    0xEF

#define ATA_STATUS_BUSY         0x80            /* Busy flag indicating internal operation */
#define ATA_STATUS_DRDY         0x40            /* Drive ready, set when drive is up to speed and ready to accept commands */
#define ATA_STATUS_WFAULT       0x20            /* Drive write fault */
#define ATA_STATUS_SEEKCOMP     0x10            /* Drive seek complete */
#define ATA_STATUS_DATA_READY   0x08            /* Indicates that the drive is ready for data transfer */
#define ATA_STATUS_CORRECTED    0x04            /* Set when there was a correctable data error found */
#define ATA_STATUS_INDEX        0x02            /* Set once per disk revolution, can determine rotational speed */
#define ATA_STATUS_ERROR        0x01            /* Set whenever an error occured */

#define ATA_SECTOR_SIZE         512
#define ATA_TIMEOUT             100000


/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

typedef struct {
	uint8_t		exists;
	uint8_t		dNum;
	uint16_t	signature;
	uint16_t	cylinders;
	uint16_t	heads;
	uint16_t	numSecsInTrack;
	uint32_t	size;				// size in sectors
} ide_devices;


extern DSTATUS g_akedrive_status[FF_VOLUMES];
extern FATFS g_fat_fs_workarea[FF_VOLUMES];

/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize(BYTE pdrv);
DSTATUS disk_status(BYTE pdrv);
DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff);
DWORD   get_fattime(void);

void f_perror(int errno);

int initialise_disk(void);


/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

typedef struct
{
    uint16_t general_config;             /*   0: General config bit-significant information          */
    uint16_t log_cyls;                   /*   1: Number of logical cylinders                         */
    uint16_t specific_config;            /*   2: Specific configuration                              */
    uint16_t log_heads;                  /*   3: Number of logical heads                             */
    uint16_t _word_4;                    /*   4: [retired]                                           */
    uint16_t _word_5;                    /*   5: [retired]                                           */
    uint16_t log_sects_per_log_track;    /*   6: Number of logical sectors per logical track         */
    uint16_t _word_7;                    /*   7: [reserved]                                          */
    uint16_t _word_8;                    /*   8: [reserved]                                          */
    uint16_t _word_9;                    /*   9: [retired]                                           */
    char serial_number[20];         /*  10: Serial number                                       */
    uint16_t _word_20;                   /*  20: [retired]                                           */
    uint16_t _word_21;                   /*  21: [retired]                                           */
    uint16_t _word_22;                   /*  22: [obsolete]                                          */
    char firmware_revision[8];      /*  23: Firmware revision                                   */
    char model_number[40];          /*  27: Model number                                        */
    uint16_t sects_per_int_multi;        /*  47: Max # sectors xferred per interrupt in R/W MULTI    */
    uint16_t _word_48;                   /*  48: [reserved]                                          */
    uint16_t capabilities_1;             /*  49: Capabilities 1                                      */
    uint16_t capabilities_2;             /*  50: Capabilities 2                                      */
    uint16_t _word_51;                   /*  51: [retired]                                           */
    uint16_t _word_52;                   /*  52: [obsolete]                                          */
    uint16_t capabilities_3;             /*  53: Capabilities 3                                      */
    uint16_t current_log_cyls;           /*  54: Number of current logical cylinders                 */
    uint16_t current_log_heads;          /*  55: Number of current logical heads                     */
    uint16_t current_log_sects_per_track;/*  56: Number of current logical sectors per track         */
    uint16_t current_capacity_sects_1;   /*  57: Current capacity in sectors (1/2)                   */
    uint16_t current_capacity_sects_2;   /*  58: Current capacity in sectors (2/2)                   */
    uint16_t current_sects_per_int_multi;/*  59: Current # sectors/int xferred in R/W MULTI          */
    uint16_t user_addressable_sects_1;   /*  60: Total # user-addressable sectors in LBA mode (1/2)  */
    uint16_t user_addressable_sects_2;   /*  61: Total # user-addressable sectors in LBA mode (2/2)  */
    uint16_t _word_62;                   /*  62: [obsolete]                                          */
    uint16_t multiword_dma_level;        /*  63: Multiword DMA supported/selected level              */
    uint16_t advanced_pio;               /*  64: Advanced PIO mode support                           */
    uint16_t min_multiword_dma_tcycle;   /*  65: Min multiword DMA t(cycle)/ns                       */
    uint16_t rec_multiword_dma_tcycle;   /*  66: Recommended multiword DMA t(cycle)/ns               */
    uint16_t min_pio_tcycle_no_flow_ctl; /*  67: Min PIO xfer t(cycle)/ns without flow control       */
    uint16_t min_pio_tcycle_iordy;       /*  68: Min PIO xfer t(cycle)/ns with IORDY flow control    */
    uint16_t _reserved_1[6];             /*  69-74 [reserved]                                        */
    uint16_t queue_depth;                /*  75: Maximum queue depth - 1                             */
    uint16_t _reserved_2[4];             /*  76-79: [reserved]                                       */
    uint16_t major_version_number;       /*  80: Major version number                                */
    uint16_t minor_version_number;       /*  81: Minor version number                                */
    uint16_t cmd_set_supported_1;        /*  82: Command set supported (1/3)                         */
    uint16_t cmd_set_supported_2;        /*  83: Command set supported (2/3)                         */
    uint16_t cmd_set_supported_3;        /*  84: Command set supported (3/3)                         */
    uint16_t cmd_feature_enabled_1;      /*  85: Command set / feature enabled (1/2)                 */
    uint16_t cmd_feature_enabled_2;      /*  86: Command set / feature enabled (2/2)                 */
    uint16_t cmd_feature_default;        /*  87: Command set / feature default                       */
    uint16_t udma_level;                 /*  88: UDMA supported/selected level                       */
    uint16_t security_erase_time;        /*  89: Time required for SECURITY ERASE UNIT               */
    uint16_t enh_security_erase_time;    /*  90: Time required for ENHANCED SECURITY ERASE UNIT      */
    uint16_t current_apm;                /*  91: Current advanced power management value             */
    uint16_t master_password_rev;        /*  92: Master password revision code                       */
    uint16_t hw_reset_result;            /*  93: Hardware reset result                               */
    uint16_t _reserved_3[33];            /*  94-126: [reserved]                                      */
    uint16_t rm_notification_supported;  /* 127: Remov. media status notif'n feature set supported   */
    uint16_t security_status;            /* 128: Security status                                     */
    uint16_t _vendor_specific_1[31];     /* 129-159: [vendor-specific]                               */
    uint16_t cfa_power_mode_1;           /* 160: CFA power mode 1                                    */
    uint16_t _reserved_4[94];            /* 161-254: [reserved]                                      */
    uint16_t integrity_word;             /* 255: Integrity word                                      */
} ata_identify_info;

#endif
