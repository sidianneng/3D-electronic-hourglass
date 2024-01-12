#include <string.h>
#include "ledcube_dis_ctl.h"

static uint8_t *cube_data = NULL;

int8_t cube_Init(uint8_t *data, uint8_t size)
{
    if(!data)
        return -1;
    if(!size)
	return -2;

    memset(data, 0x00, size);
    cube_data = data;
    return 0;
}

int8_t cube_SetXYZ(uint8_t x, uint8_t y, uint8_t z, uint8_t state)
{
    if(!cube_data) {
	printf("cube data not initlized!\n");
	return -1;
    }
    if(x >= 8 || y >= 8 || z >= 8)
	return -1;

    if (state == 1)
        cube_data[z * 8 + x] |= (0x01 << y);
    else if (state == 0)
	cube_data[z * 8 + x] &= (~(0x01 << y));

    return 0;
}

int8_t cube_GetXYZ(uint8_t x, uint8_t y, uint8_t z)
{
    if(!cube_data) {
	printf("cube data not initlized!\n");
	return -1;
    }

    if(x >= 8 || y >= 8 || z >= 8)
	return -1;

    int8_t state = !!(cube_data[z * 8 + x] & (0x01 << y));

    return state;
}
