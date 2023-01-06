#include "expansion.h"
#include <printf.h>
#include <string.h>

#include "drivers/sys/duart.h"
#include "display.h"


uint8_t g_exp_num_devices;
uint8_t g_exp_devicemap;
expansion_dev_t g_expansion_slots[MAX_EXP_DEVICES];

static const uint32_t g_exp_baseaddr[] = {EXP1_BASE, EXP2_BASE, EXP3_BASE, EXP4_BASE};
static const uint32_t g_exp_dataaddr[] = {EXP1_DATA, EXP2_DATA, EXP3_DATA, EXP4_DATA};

typedef struct
{
    uint8_t id;
    char *  id_str;
} devid_t;

static const devid_t g_device_ids[] = {
    { DEVICE_UNKNOWN,   "Unknown device" },
    { DEVICE_V9958,     "Yamaha V9958 VDP" },
    { DEVICE_V9990,     "Yamaha V9990 VDP" },
    { DEVICE_FPGA,      "Y Ddraig VGA VDP" },
    { DEVICE_TMS9918,   "Yamaha TMS9918 VDP" },
    { DEVICE_YM2151,    "Yamaha YM2151 FM Synth" },
    { DEVICE_YM2612,    "Yamaha YM2612 FM Synth" },
    { DEVICE_RTL8019,   "Realtek RTL8019AS Ethernet" },
    { DEVICE_FDC,       "WD37C65C DC" },
    { DEVICE_MD_JOYPAD, "Sega Megadrive Joypad" },
};

static void expansion_delay(unsigned long d)
{
    volatile unsigned long wait = d;
    while (wait--);
}

uint8_t read_exp_id(uint8_t dev)
{
    switch (dev)
    {
        case EXP_ID1:
            return EXPID_REG(EXP1_BASE, EXPID_IDENTREG);
        case EXP_ID2:
            return EXPID_REG(EXP2_BASE, EXPID_IDENTREG);
        case EXP_ID3:
            return EXPID_REG(EXP3_BASE, EXPID_IDENTREG);
        case EXP_ID4:
            return EXPID_REG(EXP4_BASE, EXPID_IDENTREG);
    }

    return 0;
}

uint8_t is_exp_board_present(uint8_t slot)
{
    if (g_exp_devicemap & (1 << slot))
        return 1;

    return 0;
}

void init_expansion_bus(void)
{
    // TODO. Scan through the connected devices and initialise as found.
    //       Fill in the data srtucture for the devices and also for the specific device initialisation.
    //       If one video device has been initialised, don't initialise another, same for sound or similar devices.
    //       Implement a way for user programs to access this data and be able to control the devices directly.

    memset(g_expansion_slots, 0, sizeof(expansion_dev_t) * MAX_EXP_DEVICES);

    g_exp_num_devices = 0;

    uint8_t devs = read_serial_inputs() >> 2;
    g_exp_devicemap = ~devs & 0x0F;

    for (int i = 0; i < MAX_EXP_DEVICES; i++)
    {
        if (is_exp_board_present(i))
        {
            uint8_t id;
            int8_t retries = 10;
            while (retries--)
            {
                id = read_exp_id(i);
                // FPGA VGA Card can take a little time to initialise,
                // ID will be 0 until it powers up.
                if (id == 0)
                    expansion_delay(50000);
                else
                {
                    expansion_dev_t *device = &g_expansion_slots[i];
                    device->device_id = id;
                    device->slot_id = i;
                    device->exp_base_address = g_exp_baseaddr[i];
                    device->exp_data_address = g_exp_dataaddr[i];
                    device->device_type = id & 0xF0;
                    init_expansion_device(device);
                    g_exp_num_devices++;
                    break;
                }
            }
        }
    }
}

void init_expansion_device(expansion_dev_t *device)
{
    switch(device->device_type)
    {
        case DEVTYPE_VIDEO:
            if (has_display())
            {
                debug_printf("Video already initialised, skipping %s in slot %d\n", get_expid_description(device->slot_id), device->slot_id + 1);
                break;
            }
            init_display(device);
            break;
        case DEVTYPE_AUDIO:
            // Init audio
            break;
        case DEVTYPE_NETWORK:
            // init network
            break;
        case DEVTYPE_IO:
            // Init IO device
            break;
        default:
            debug_printf("Unknown device type found on slot %d, device id = %02X\n", device->slot_id, device->device_id);
            break;
    }
}

char *get_expid_description(uint8_t slot)
{
    uint8_t dev_id = 0;
    if (slot < MAX_EXP_DEVICES)
        dev_id = get_expid_ident(slot);

    int num_devices = sizeof(g_device_ids) / sizeof(devid_t);
    for (int i = 0; i < num_devices; i++)
    {
        if (g_device_ids[i].id == dev_id)
            return g_device_ids[i].id_str;
    }

    return "Unexpected device found";
}

uint32_t get_expid_base_address(uint8_t slot)
{
    if (slot < MAX_EXP_DEVICES)
        return g_exp_baseaddr[slot];

    return 0;
}

uint32_t get_expid_data_address(uint8_t slot)
{
    if (slot < MAX_EXP_DEVICES)
        return g_exp_dataaddr[slot];

    return 0;
}

uint32_t get_expid_int_vect_slot(uint8_t slot)
{
    switch (slot)
    {
        case 0:
            return 0x64;
        case 1:
            return 0x78;
        case 2:
            return 0x7C;
    }

    return 0;
}

uint32_t get_expid_int_vect_addr(uint32_t addr)
{
    switch (addr)
    {
        case EXP1_BASE:
            return 0x64;
        case EXP2_BASE:
            return 0x78;
        case EXP3_BASE:
            return 0x7C;
    }

    return 0;
}


uint8_t get_expid_ident(uint8_t slot)
{
    uint8_t dev_id = 0;
    if (slot < MAX_EXP_DEVICES)
        dev_id = g_expansion_slots[slot].device_id;

    return dev_id;
}

uint8_t num_exp_boards_installed(void)
{
    return g_exp_num_devices;
}

expansion_dev_t *get_expansion_info(uint8_t slot)
{
    if (slot >= MAX_EXP_DEVICES)
        return NULL;

    return &g_expansion_slots[slot];
}
