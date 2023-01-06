#ifndef DDRAIG_OS_NETWORK_H
#define DDRAIG_OS_NETWORK_H

#include "ddraig.h"

struct network_device;

typedef struct network_device
{
    uint32_t base_address;

    bool (*device_init)(struct network_device *display);

} network_device_t;


bool init_network(uint8_t slot);

network_device_t *get_network_info();

void reset_network();


#endif
