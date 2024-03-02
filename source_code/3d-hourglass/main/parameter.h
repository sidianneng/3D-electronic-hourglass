typedef struct {
	uint16_t port;
	char ipv4[20]; // xxx.xxx.xxx.xxx
} PARAMETER_t;


typedef struct {
	float quatx;
	float quaty;
	float quatz;
	float quatw;
	float roll;
	float pitch;
	float yaw;
} POSE_t;

#ifdef __cplusplus
extern "C" {
#endif

#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD 0.0174533

void mpu6050(void *pvParameters);

#ifdef __cplusplus
}
#endif
