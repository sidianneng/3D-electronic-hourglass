#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int8_t cube_Init(uint8_t *data, uint8_t size);
int8_t cube_SetXYZ(uint8_t x, uint8_t y, uint8_t z, uint8_t state);
int8_t cube_GetXYZ(uint8_t x, uint8_t y, uint8_t z);

#ifdef __cplusplus
}
#endif
