#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum HG_ID {
    HG_TOP = 0,
    HG_BOTTOM,
};
typedef enum HG_ID Hg_Id;

enum HG_STATE {
    HG_UP = 0,
    HG_DOWN,
};
typedef enum HG_STATE Hg_state;

int8_t hg_MoveSand(Hg_Id id, Hg_state state, int8_t x, int8_t y, int8_t z);
int8_t hg_Update(void);

#ifdef __cplusplus
}
#endif
