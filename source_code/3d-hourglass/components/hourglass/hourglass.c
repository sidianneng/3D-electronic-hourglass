#include <string.h>
#include "hourglass.h"
#include "ledcube_dis_ctl.h"
#include "esp_random.h"
#include "stdbool.h"

static bool sand_CanGoX(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    bool result;
    if(id == HG_TOP) {
	if(x == 7 || x + 1 + y + z > 10)
	    return false;
	result = (state == HG_UP ? cube_GetXYZ(x + 1, y, z) : !cube_GetXYZ(x + 1, y, z));
    } else {
	if(x == -8 || x - 1 + y + z < -13)
	    return false;
	result = (state == HG_UP ? cube_GetXYZ(x - 1, y, z) : !cube_GetXYZ(x - 1, y, z));
    }

    return result;
}

static bool sand_CanGoY(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    bool result;
    if(id == HG_TOP) {
        if(y == 7 || x + y + 1 + z > 10)
            return false;
	result = (state == HG_UP ? cube_GetXYZ(x, y + 1, z) : !cube_GetXYZ(x, y + 1, z));
    } else {
        if(y == -8 || x + y - 1 + z < -13)
            return false;
	result = (state == HG_UP ? cube_GetXYZ(x, y - 1, z) : !cube_GetXYZ(x, y - 1, z));
    }
    return result;
}

static bool sand_CanGoZ(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    bool result;
    if(id == HG_TOP) {
        if(z == 7 || x + y + z + 1 > 10)
            return false;
	result = (state == HG_UP ? cube_GetXYZ(x, y, z + 1) : !cube_GetXYZ(x, y, z + 1));
    } else {
        if(z == -8 || x + y + z - 1 < -13)
            return false;
	result = (state == HG_UP ? cube_GetXYZ(x, y, z - 1) : !cube_GetXYZ(x, y, z - 1));
    }
    return result;
}

static bool sand_CanGoDown(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    bool result;
    if(id == HG_TOP){
        if(x + 1 + y + 1 + z + 1 > 10 || x == 7 || y == 7 || z == 7)
    	    return false;
        //if(sand_CanGoX(id, state, x, y, z) || sand_CanGoY(id, state, x, y, z) || sand_CanGoZ(id, state, x, y, z))
  	//    return false;
	result = (state == HG_UP ? cube_GetXYZ(x + 1, y + 1, z + 1) : !cube_GetXYZ(x + 1, y + 1, z + 1));
    } else {
        if(x - 1 + y - 1 + z - 1 < -13 || x == -8 || y == -8 || z == -8)
    	    return false;
        //if(!sand_CanGoX(id, state, x, y, z) || !sand_CanGoY(id, state, x, y, z) || !sand_CanGoZ(id, state, x, y, z))
	//    return false;
	result = (state == HG_UP ? cube_GetXYZ(x - 1, y - 1, z - 1) : !cube_GetXYZ(x - 1, y - 1, z - 1));
    }

    return result;
}

static void sand_GoDown(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    uint8_t led_state;
    int8_t step_dir;

    step_dir = (id == HG_TOP ? 1 : -1);
    led_state = (state == HG_UP ? 1 : 0);

    cube_SetXYZ(x, y, z, led_state);
    cube_SetXYZ(x + step_dir, y + step_dir, z + step_dir, !led_state);
}

static void sand_GoX(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    uint8_t led_state;
    int8_t step_dir;

    step_dir = (id == HG_TOP ? 1 : -1);
    led_state = (state == HG_UP ? 1 : 0);

    cube_SetXYZ(x, y, z, led_state);
    cube_SetXYZ(x + step_dir, y, z, !led_state);
}

static void sand_GoY(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    uint8_t led_state;
    int8_t step_dir;

    step_dir = (id == HG_TOP ? 1 : -1);
    led_state = (state == HG_UP ? 1 : 0);

    cube_SetXYZ(x, y, z, led_state);
    cube_SetXYZ(x, y + step_dir, z, !led_state);
}

static void sand_GoZ(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    uint8_t led_state;
    int8_t step_dir;

    step_dir = (id == HG_TOP ? 1 : -1);
    led_state = (state == HG_UP ? 1 : 0);

    cube_SetXYZ(x, y, z, led_state);
    cube_SetXYZ(x, y, z + step_dir, !led_state);
}

int8_t hg_MoveSand(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z)
{
    uint32_t random = 0x00;

    if(state == HG_UP) {
        if(cube_GetXYZ(x, y, z))
            return false;
    } else {
        if(!cube_GetXYZ(x, y, z))
          return false;
    }

    bool cangodown = sand_CanGoDown(id, state, x, y, z);
    bool cangox    = sand_CanGoX(id, state, x, y, z);
    bool cangoy    = sand_CanGoY(id, state, x, y, z);
    bool cangoz    = sand_CanGoZ(id, state, x, y, z);

    if(!cangox && !cangoy && !cangoz)
	return false;

    if(cangodown) {
	sand_GoDown(id, state, x, y, z);
    } else {
    	random = esp_random() % 2;
    	switch((uint16_t)cangox << 8 | (uint16_t)cangoy << 4 | (uint16_t)cangoz << 0)
    	{
    	    case 0x001:
    	        sand_GoZ(id, state, x, y, z);
    	        break;
    	    case 0x010:
    	        sand_GoY(id, state, x, y, z);
    	        break;
    	    case 0x100:
    	        sand_GoX(id, state, x, y, z);
    	        break;
    	    case 0x011:
    	        if(random == 0)
    	    	    sand_GoY(id, state, x, y, z);
    	        else
    	    	    sand_GoZ(id, state, x, y, z);
    	        break;
    	    case 0x101:
    	        if(random == 0)
    	    	    sand_GoX(id, state, x, y, z);
    	        else
    	    	    sand_GoZ(id, state, x, y, z);
    	        break;
    	    case 0x110:
    	        if(random == 0)
    	    	    sand_GoX(id, state, x, y, z);
    	        else
    	    	    sand_GoY(id, state, x, y, z);
    	        break;
    	    case 0x111:
    	        random = esp_random() % 3;
    	        if(random == 0)
    	    	    sand_GoX(id, state, x, y, z);
    	        else if(random == 1)
    	    	    sand_GoY(id, state, x, y, z);
    	        else
    	    	    sand_GoZ(id, state, x, y, z);
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
