#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int8_t cube_Init(uint8_t *data, uint8_t size);
int8_t cube_SetXYZ(int8_t x, int8_t y, int8_t z, uint8_t state);
int8_t cube_GetXYZ(int8_t x, int8_t y, int8_t z);

#ifdef __cplusplus
}
#endif
