#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

enum HG_ID {
    HG_TOP = 0,
    HG_BOTTOM,
};
typedef enum HG_ID Hg_Id;

int8_t hg_MoveSand(Hg_Id id, uint8_t x, uint8_t y, uint8_t z);
int8_t hg_Update(void);

#ifdef __cplusplus
}
#endif
