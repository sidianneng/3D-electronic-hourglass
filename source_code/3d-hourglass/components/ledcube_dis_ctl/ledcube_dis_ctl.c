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

int8_t cube_SetXYZ(int8_t x, int8_t y, int8_t z, uint8_t state)
{
    int8_t tmp_x = x, tmp_y = y, tmp_z = z;
    if(!cube_data) {
	printf("cube data not initlized!\n");
	return -1;
    }
    if(x >= 8 || y >= 8 || z >= 8)
	return -2;

    if(x < -8 || y < -8 || z < -8)
	return -3;

    if(x < 0 || y < 0 || z < 0) {
	tmp_x = x + 8;
	tmp_y = y + 8;
	tmp_z = z + 8;	
    }

    if (state == 1)
        cube_data[tmp_z * 8 + tmp_x] |= (0x01 << tmp_y);
    else if (state == 0)
	cube_data[tmp_z * 8 + tmp_x] &= (~(0x01 << tmp_y));

    return 0;
}

int8_t cube_GetXYZ(int8_t x, int8_t y, int8_t z)
{
    int8_t tmp_x = x, tmp_y = y, tmp_z = z;
    if(!cube_data) {
	printf("cube data not initlized!\n");
	return -1;
    }

    if(x >= 8 || y >= 8 || z >= 8)
	return -2;

    if(x < -8 || y < -8 || z < -8)
	return -3;

    if(x < 0 || y < 0 || z < 0) {
	tmp_x = x + 8;
	tmp_y = y + 8;
	tmp_z = z + 8;	
    }

    int8_t state = !!(cube_data[tmp_z * 8 + tmp_x] & (0x01 << tmp_y));

    return state;
}
