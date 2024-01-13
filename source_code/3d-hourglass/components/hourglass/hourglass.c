#include <string.h>
#include "hourglass.h"
#include "ledcube_dis_ctl.h"
#include "esp_random.h"
#include "stdbool.h"

static bool sand_CanGoX(uint8_t x, uint8_t y, uint8_t z)
{
    if(x == 0 || x + y + z <= 11)
        return false;
    return !cube_GetXYZ(x - 1, y, z);
}

static bool sand_CanGoY(uint8_t x, uint8_t y, uint8_t z)
{
    if(y == 0 || x + y + z <= 11)
        return false;
    return !cube_GetXYZ(x, y - 1, z);
}

static bool sand_CanGoZ(uint8_t x, uint8_t y, uint8_t z)
{
    if(z == 0 || x + y + z <= 11)
        return false;
    return !cube_GetXYZ(x, y, z - 1);
}

static bool sand_CanGoDown(uint8_t x, uint8_t y, uint8_t z)
{
    if(x + y + z <= 11 || x == 0 || y == 0 || z == 0)
	return false;

    if(!sand_CanGoX(x, y, z))
	return false;
    if(!sand_CanGoY(x, y, z))
	return false;
    if(!sand_CanGoZ(x, y, z))
	return false;

    return !cube_GetXYZ(x - 1, y - 1, z - 1);
}

static void sand_GoDown(uint8_t x, uint8_t y, uint8_t z)
{
    cube_SetXYZ(x, y, z, 0);
    cube_SetXYZ(x - 1, y - 1, z - 1, 1);
}

static void sand_GoX(uint8_t x, uint8_t y, uint8_t z)
{
    cube_SetXYZ(x, y, z, 0);
    cube_SetXYZ(x - 1, y, z, 1);
}

static void sand_GoY(uint8_t x, uint8_t y, uint8_t z)
{
    cube_SetXYZ(x, y, z, 0);
    cube_SetXYZ(x, y - 1, z, 1);
}

static void sand_GoZ(uint8_t x, uint8_t y, uint8_t z)
{
    cube_SetXYZ(x, y, z, 0);
    cube_SetXYZ(x, y, z - 1, 1);
}

int8_t hg_MoveSand(uint8_t x, uint8_t y, uint8_t z)
{
    uint32_t random = 0x00;

    if(!cube_GetXYZ(x, y, z))
	return false;

    bool cangodown = sand_CanGoDown(x, y, z);
    bool cangox    = sand_CanGoX(x, y, z);
    bool cangoy    = sand_CanGoY(x, y, z);
    bool cangoz    = sand_CanGoZ(x, y, z);

    if(!cangox && !cangoy && !cangoz)
	return false;

    if(cangodown) {
	sand_GoDown(x, y, z);
    } else {
    	random = esp_random() % 2;
    	switch((uint16_t)cangox << 8 | (uint16_t)cangoy << 4 | (uint16_t)cangoz << 0)
    	{
    	    case 0x001:
    	        sand_GoZ(x, y, z);
    	        break;
    	    case 0x010:
    	        sand_GoY(x, y, z);
    	        break;
    	    case 0x100:
    	        sand_GoX(x, y, z);
    	        break;
    	    case 0x011:
    	        if(random == 0)
    	    	    sand_GoY(x, y, z);
    	        else
    	    	    sand_GoZ(x, y, z);
    	        break;
    	    case 0x101:
    	        if(random == 0)
    	    	    sand_GoX(x, y, z);
    	        else
    	    	    sand_GoZ(x, y, z);
    	        break;
    	    case 0x110:
    	        if(random == 0)
    	    	    sand_GoX(x, y, z);
    	        else
    	    	    sand_GoY(x, y, z);
    	        break;
    	    case 0x111:
    	        random = esp_random() % 3;
    	        if(random == 0)
    	    	    sand_GoX(x, y, z);
    	        else if(random == 1)
    	    	    sand_GoY(x, y, z);
    	        else
    	    	    sand_GoZ(x, y, z);
    	        break;
    	    default:
    	        break;
    	}
    }

    return true;
}

int8_t hg_Update(void)
{
    return 0;
}
