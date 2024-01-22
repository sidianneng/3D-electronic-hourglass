#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOSConfig.h"
#include "driver/gpio.h"
#include "ledcube_dis_ctl.h"
#include "hourglass.h"
#include "driver/i2c.h"
#include "parameter.h"
#include <math.h>

//active low
int rd_gpios[8] = {
    CONFIG_GPIO_RD_0,
    CONFIG_GPIO_RD_1,
    CONFIG_GPIO_RD_2,
    CONFIG_GPIO_RD_3,
    CONFIG_GPIO_RD_4,
    CONFIG_GPIO_RD_5,
    CONFIG_GPIO_RD_6,
    CONFIG_GPIO_RD_7,
};

//active low
int g_gpios[8] = {
    CONFIG_GPIO_G_0,
    CONFIG_GPIO_G_1,
    CONFIG_GPIO_G_2,
    CONFIG_GPIO_G_3,
    CONFIG_GPIO_G_4,
    CONFIG_GPIO_G_5,
    CONFIG_GPIO_G_6,
    CONFIG_GPIO_G_7,
};

//active high
int l_gpios[8] = {
    CONFIG_GPIO_L_0,
    CONFIG_GPIO_L_1,
    CONFIG_GPIO_L_2,
    CONFIG_GPIO_L_3,
    CONFIG_GPIO_L_4,
    CONFIG_GPIO_L_5,
    CONFIG_GPIO_L_6,
    CONFIG_GPIO_L_7,
};

static QueueHandle_t ledcube_data_queue = NULL;

static uint8_t ledcube_data[64] = {0x0};

extern float ypr[3];

static void led_cube_display(void* arg)
{
    static uint8_t data_tmp = 0x00;

    //get new ledcube data from queue

    //display ledcube
    for(;;) {
        for(uint8_t level = 0;level < 8; level++) {
	    for(uint8_t line = 0;line < 8; line++) {
	        data_tmp = ledcube_data[level * 8 + line];
	        for(uint8_t bit = 0;bit < 8; bit++) {
	            gpio_set_level((gpio_num_t)rd_gpios[bit], !(data_tmp & 0x01));
		    data_tmp = (data_tmp >> 1);
                }
	        gpio_set_level((gpio_num_t)l_gpios[line], 1);
	        gpio_set_level((gpio_num_t)l_gpios[line], 0);
	    }
	    for(uint8_t temp = 0;temp < 8; temp++) {
	        if(temp == level)
	            gpio_set_level((gpio_num_t)g_gpios[temp], 0);
	        else
	            gpio_set_level((gpio_num_t)g_gpios[temp], 1);
	    }
	    vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    }
}

void start_i2c(void) {
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)CONFIG_GPIO_SDA;
	conf.scl_io_num = (gpio_num_t)CONFIG_GPIO_SCL;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	conf.clk_flags = 0;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

void app_main(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    for(uint8_t i = 0;i < 8; ++i)
        io_conf.pin_bit_mask |= ((1ULL << rd_gpios[i]) | (1ULL << g_gpios[i]) | (1ULL << l_gpios[i]));
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t)0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t)0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //Init i2c
    start_i2c();

    int cnt = 0;
    int8_t led_state = 0;
    cube_Init(ledcube_data, sizeof(ledcube_data));

    //close all the leds
    for(uint8_t i = 0;i < 8; ++i) {
	gpio_set_level((gpio_num_t)g_gpios[i], 1);
        gpio_set_level((gpio_num_t)rd_gpios[i], 1);
    }
    for(uint8_t i = 0;i < 8; ++i) {
        gpio_set_level((gpio_num_t)l_gpios[i], 1);
    }

    //create the data queue for led cube
    ledcube_data_queue = xQueueCreate(64, sizeof(uint8_t)); 
    //create the task for LED display 
    xTaskCreate(led_cube_display, "led_cube_display", 2048, NULL, 10, NULL);

    //create the task for IMU
    xTaskCreate(mpu6050, "IMU", 1024*8, NULL, 10, NULL);

    //plane equation x+y+z=10 for hourglass top
    for(uint8_t h = 0;h <= 5; h++){
    for(uint8_t i = 0;i < 8; i++)
        for(uint8_t j = 0;j < 8; j++)
            for(uint8_t k = 0;k < 8; k++) {
        	if(i + j + k == h)
        	    cube_SetXYZ(i, j, k, 1);
            }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    uint8_t h, i, j, k;
    //cube_SetXYZ(7, 7, 7, 1);
    float A, B, C;
    float yaw, pitch, roll;
    uint8_t led_num = 0;
    while(1) {
	vTaskDelay(50 / portTICK_PERIOD_MS);
        //for(i = 0;i < 8; i++)
	//    for(j = 0;j < 8; j++)	
	//	for(k = 0;k < 8; k++){
	//            if(i + j + k <= 10)
	//		hg_MoveSand(HG_TOP, i, j, k);
	//	}
        //for(i = 0;i < 8; i++)
	//    for(j = 0;j < 8; j++)	
	//	for(k = 0;k < 8; k++){
	//            if(i + j + k >= 11)
	//		hg_MoveSand(HG_BOTTOM, i, j, k);
	//	}
        for(uint8_t h = 0;h <= 22; h++){
        for(uint8_t i = 0;i < 8; i++)
            for(uint8_t j = 0;j < 8; j++)
                for(uint8_t k = 0;k < 8; k++) {
            	if(i + j + k == h)
            	    cube_SetXYZ(i, j, k, 0);
                }
        }
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
	printf("cnt:%d\n", cnt++);
	//printf("yaw:%f pitch:%f roll:%f\n", ypr[0], ypr[1], ypr[2]);
	yaw = ypr[0];
	pitch= ypr[1];
	roll = ypr[2];
	A = cos(yaw)*cos(roll) + (sin(pitch)*sin(yaw)*cos(roll) - cos(pitch)*sin(roll)) + (sin(yaw)*cos(pitch)*cos(roll) + sin(pitch)*sin(roll));
	B = cos(yaw)*sin(roll) + (sin(pitch)*sin(yaw)*sin(roll) + cos(pitch)*cos(roll)) + (sin(yaw)*sin(roll)*cos(pitch) - sin(pitch)*cos(roll));
	C = (-1)*sin(yaw) + sin(pitch)*cos(yaw) + cos(pitch)*cos(yaw);
	printf("A:%f B:%f C:%f\n", A, B, C);
        for(uint8_t h = 0;h <= 4; h++){
            for(uint8_t i = 0;i < 8; i++)
                for(uint8_t j = 0;j < 8; j++)
                    for(uint8_t k = 0;k < 8; k++) {
			if(A*i + B*j + C*k <= h) {
			    cube_SetXYZ(i, j, k, 1);
			    led_num++;
			    if(led_num >= 56) {
				led_num = 0;
				break;
			    }
			}
		    }
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	//if(cnt % 2 == 0) {
        //    cube_SetXYZ(7, 7, 7, 1);
	//    cube_SetXYZ(0, 0, 0, 0);
	//}
    }
}
