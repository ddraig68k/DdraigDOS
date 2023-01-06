#ifndef DDRAIG_OS_EXPANSION_H
#define DDRAIG_OS_EXPANSION_H

#include "ddraig.h"

#define MAX_EXP_DEVICES     4

#define EXP_ID1             0
#define EXP_ID2             1
#define EXP_ID3             2
#define EXP_ID4             3

#define DEVTYPE_UNKNOWN     0x00
#define DEVTYPE_VIDEO       0x10
#define DEVTYPE_AUDIO       0x20
#define DEVTYPE_NETWORK     0x30
#define DEVTYPE_IO          0x40

#define DEVICE_UNKNOWN      0x00
#define DEVICE_V9958        (DEVTYPE_VIDEO + 1)
#define DEVICE_V9990        (DEVTYPE_VIDEO + 2)
#define DEVICE_FPGA         (DEVTYPE_VIDEO + 3)
#define DEVICE_TMS9918      (DEVTYPE_VIDEO + 4)
#define DEVICE_YM2151       (DEVTYPE_AUDIO + 1)
#define DEVICE_YM2612       (DEVTYPE_AUDIO + 2)
#define DEVICE_RTL8019      (DEVTYPE_NETWORK + 1)
#define DEVICE_FDC          (DEVTYPE_IO + 1)
#define DEVICE_MD_JOYPAD    (DEVTYPE_IO + 2)

typedef struct
{
    uint32_t exp_base_address;      /* register access address for the slot */
    uint32_t exp_data_address;      /* direct data access address for the slot */
    void *exp_driver;               /* pointer to the driver structure for the expansion device */
    uint8_t slot_id;                /* slot number for the device */
    uint8_t device_id;              /* detected device ID, 0 if no device detected */
    uint8_t device_type;            /* detected expansion device type */
} expansion_dev_t;

void init_expansion_bus(void);
void init_expansion_device(expansion_dev_t *device);

uint8_t read_exp_id(uint8_t dev);
uint8_t is_exp_board_present(uint8_t slot);
char *get_expid_description(uint8_t slot);
uint8_t get_expid_ident(uint8_t slot);
expansion_dev_t *get_expansion_info(uint8_t slot);

uint32_t get_expid_base_address(uint8_t slot);
uint32_t get_expid_data_address(uint8_t slot);
uint32_t get_expid_int_vect_slot(uint8_t slot);
uint32_t get_expid_int_vect_addr(uint32_t addr);

uint8_t num_exp_boards_installed(void);

#endif
