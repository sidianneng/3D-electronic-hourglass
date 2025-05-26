#include <string.h>
#include "ledcube_dis_ctl.h"

static uint8_t *cube_data = NULL;

#define DEPOS 0x000 //default position for the leds whose can't be turned on in the hardware.
uint32_t leds_convert[8][8][8] = {
    {
        {0x000,0x001,0x002,0x003,0x004,0x005,0x006,0x007},//(0,0,0)~(0,0,7)
        {0x010,0x011,0x012,0x013,DEPOS,0x015,0x016,0x017},//(0,1,0)~(0,1,7)
        {0x020,0x021,0x022,0x023,0x024,0x025,0x026,0x027},//(0,2,0)~(0,2,7)
        {DEPOS,DEPOS,DEPOS,DEPOS,DEPOS,DEPOS,DEPOS,DEPOS},//(0,3,0)~(0,3,7)
        {0x077,0x601,0x602,0x603,0x604,0x605,0x606,DEPOS},//(0,4,0)~(0,4,7)
        {0x067,DEPOS,0x502,0x503,0x504,0x505,DEPOS,DEPOS},//(0,5,0)~(0,5,7)
        {0x057,0x066,0x402,0x403,0x404,DEPOS,DEPOS,DEPOS},//(0,6,0)~(0,6,7)
        {0x047,0x056,DEPOS,DEPOS,0x044,DEPOS,DEPOS,DEPOS},//(0,7,0)~(0,7,7)
    },
    {
        {0x100,0x101,0x102,0x103,0x104,0x105,0x106,0x107},//(1,0,0)~(1,0,7)
        {0x110,0x111,0x112,0x113,0x114,0x115,0x116,0x117},//(1,1,0)~(1,1,7)
        {DEPOS,0x711,0x712,0x713,0x714,0x715,0x716,DEPOS},//(1,2,0)~(1,2,7)
        {DEPOS,0x611,0x612,0x613,DEPOS,0x615,0x706,0x177},//(1,3,0)~(1,3,7)
        {DEPOS,0x511,0x512,0x513,0x514,0x515,0x166,0x167},//(1,4,0)~(1,4,7)
        {DEPOS,DEPOS,0x412,0x413,0x414,0x155,0x156,0x157},//(1,5,0)~(1,5,7)
        {DEPOS,DEPOS,0x312,0x313,0x144,0x145,0x146,0x147},//(1,6,0)~(1,6,7)
        {DEPOS,DEPOS,DEPOS,DEPOS,0x134,0x135,0x136,0x137},//(1,7,0)~(1,7,7)
    },
    {
        {0x200,0x201,0x202,0x203,0x204,0x205,0x206,0x207},//(2,0,0)~(2,0,7)
        {DEPOS,0x721,0x722,0x723,0x724,0x725,0x726,DEPOS},//(2,1,0)~(2,1,7)
        {DEPOS,0x621,0x622,0x623,0x624,0x625,DEPOS,DEPOS},//(2,2,0)~(2,2,7)
        {DEPOS,0x521,DEPOS,0x523,0x524,0x525,DEPOS,DEPOS},//(2,3,0)~(2,3,7)
        {DEPOS,0x421,DEPOS,0x423,0x424,0x255,0x256,0x257},//(2,4,0)~(2,4,7)
        {0x320,DEPOS,0x322,DEPOS,0x244,0x245,0x246,0x247},//(2,5,0)~(2,5,7)
        {DEPOS,DEPOS,DEPOS,0x233,0x234,0x235,0x236,0x237},//(2,6,0)~(2,6,7)
        {DEPOS,DEPOS,DEPOS,DEPOS,DEPOS,DEPOS,DEPOS,DEPOS},//(2,7,0)~(2,7,7)
    },
    {
        {DEPOS,0x311,0x732,0x733,0x734,0x735,0x736,0x437},//(3,0,0)~(3,0,7)
        {DEPOS,0x631,0x632,0x633,0x634,0x635,0x636,0x377},//(3,1,0)~(3,1,7)
        {DEPOS,0x531,0x532,DEPOS,DEPOS,DEPOS,0x366,0x367},//(3,2,0)~(3,2,7)
        {DEPOS,0x431,0x432,0x433,0x434,DEPOS,DEPOS,0x357},//(3,3,0)~(3,3,7)
        {0x330,0x331,0x332,0x333,0x344,0x345,0x346,0x347},//(3,4,0)~(3,4,7)
        {0x230,0x231,0x232,DEPOS,0x334,0x335,0x336,0x337},//(3,5,0)~(3,5,7)
        {DEPOS,DEPOS,DEPOS,DEPOS,0x324,0x325,0x326,0x327},//(3,6,0)~(3,6,7)
        {0x130,DEPOS,DEPOS,DEPOS,0x314,0x315,0x316,0x317},//(3,7,0)~(3,7,7)
    },
    {
        {},//(4,0,0)~(4,0,7)
        {},//(4,1,0)~(4,1,7)
        {},//(4,2,0)~(4,2,7)
        {},//(4,3,0)~(4,3,7)
        {},//(4,4,0)~(4,4,7)
        {},//(4,5,0)~(4,5,7)
        {},//(4,6,0)~(4,6,7)
        {},//(4,7,0)~(4,7,7)
    },
    {
        {},//(5,0,0)~(5,0,7)
        {},//(5,1,0)~(5,1,7)
        {},//(5,2,0)~(5,2,7)
        {},//(5,3,0)~(5,3,7)
        {},//(5,4,0)~(5,4,7)
        {},//(5,5,0)~(5,5,7)
        {},//(5,6,0)~(5,6,7)
        {},//(5,7,0)~(5,7,7)
    },
    {
        {},//(6,0,0)~(6,0,7)
        {},//(6,1,0)~(6,1,7)
        {},//(6,2,0)~(6,2,7)
        {},//(6,3,0)~(6,3,7)
        {},//(6,4,0)~(6,4,7)
        {},//(6,5,0)~(6,5,7)
        {},//(6,6,0)~(6,6,7)
        {},//(6,7,0)~(6,7,7)
    },
    {
        {},//(7,0,0)~(7,0,7)
        {},//(7,1,0)~(7,1,7)
        {},//(7,2,0)~(7,2,7)
        {},//(7,3,0)~(7,3,7)
        {},//(7,4,0)~(7,4,7)
        {},//(7,5,0)~(7,5,7)
        {},//(7,6,0)~(7,6,7)
        {},//(7,7,0)~(7,7,7)
    },
};

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

    x = ((leds_convert[tmp_x][tmp_y][tmp_z] >> 8) & 0xf);
    y = ((leds_convert[tmp_x][tmp_y][tmp_z] >> 4) & 0xf);
    z = ((leds_convert[tmp_x][tmp_y][tmp_z] >> 0) & 0xf);
    tmp_x = x;
    tmp_y = y;
    tmp_z = z;

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

    x = ((leds_convert[tmp_x][tmp_y][tmp_z] >> 8) & 0xf);
    y = ((leds_convert[tmp_x][tmp_y][tmp_z] >> 4) & 0xf);
    z = ((leds_convert[tmp_x][tmp_y][tmp_z] >> 0) & 0xf);
    tmp_x = x;
    tmp_y = y;
    tmp_z = z;

    int8_t state = !!(cube_data[tmp_z * 8 + tmp_x] & (0x01 << tmp_y));

    return state;
}
