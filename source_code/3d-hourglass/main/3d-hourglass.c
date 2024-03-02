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
    int8_t h, i, j, k;
    //for(h = -3;h >= -13; h--){
    //    for(i = -1;i >= -8; i--) {
    //        for(j = -1;j >= -8; j--) {
    //            for(k = -1;k >= -8; k--) {
    //        	if(i + j + k == h)
    //        	    cube_SetXYZ(i, j, k, 1);
    //            }
    //        }
    //    }
    //}
    for(h = 0;h <= 10; h++){
        for(i = 0;i < 8; i++) {
            for(j = 0;j < 8; j++) {
                for(k = 0;k < 8; k++) {
            	if(i + j + k == h)
            	    cube_SetXYZ(i, j, k, 1);
                }
            }
        }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    float yaw;
    int16_t top_sand_num = 0, bottom_sand_num = 0;
#define UP2DOWN 0
#define DOWN2UP 1
#define FALLDOWN 2
    uint8_t previous_orient = UP2DOWN;
    uint8_t current_orient = UP2DOWN;
    uint16_t top_led_num = 0, bottom_led_num = 0;
    uint8_t trigger_led_state = 0;
    while(1) {
	vTaskDelay(100 / portTICK_PERIOD_MS);
	yaw = ypr[0] * RAD_TO_DEG;
	printf("yaw:%f\n", yaw);
	if (yaw <= 45.0 && yaw >= -45.0) { //up to down
		current_orient = UP2DOWN;
		trigger_led_state = 0;
	} else if((yaw >= 135 && yaw <= 180) || (yaw <= -135 && yaw >= -180)) {//down to up
		current_orient = DOWN2UP;
		trigger_led_state = 1;
	} else {//fall down
		current_orient = FALLDOWN;
	}

	//we need to redraw the hourglass
	if((current_orient == UP2DOWN || current_orient == DOWN2UP) && current_orient != previous_orient) {
        	for(i = -8;i <= -1; i++)
        	    for(j = -8;j <= -1; j++)
        	        for(k = -8;k <= -1; k++)
        	            if(i + j + k >= -13)
        	                    bottom_sand_num += cube_GetXYZ(i, j, k);
		top_sand_num = 256 - bottom_sand_num;

		//close all the leds
        	for(uint8_t i = 0;i < 8; i++)
        	    for(uint8_t j = 0;j < 8; j++)
        	        for(uint8_t k = 0;k < 8; k++) 
        	    	    cube_SetXYZ(i, j, k, 0);

		if(current_orient == DOWN2UP) {
			//draw the down part
    			for(h = -3;h >= -13; h--) {
    			    for(i = -1;i >= -8; i--) {
    			        for(j = -1;j >= -8; j--) {
    			            for(k = -1;k >= -8; k--) {
    			                if(i + j + k == h) {
    			                    cube_SetXYZ(i, j, k, 1);
					    bottom_led_num++;
					    if(bottom_led_num >= bottom_sand_num) {
					        bottom_led_num = 0;
						goto down_draw_finish1; 
					    }
					}
    			            }
    			        }
    			    }
    			}
down_draw_finish1:
			//draw the top part
			for(h = 10;h >= 0; h--){
			    for(i = 0;i <= 7; i++) {
			        for(j = 0;j <= 7; j++) {
			            for(k = 0;k <= 7; k++) {
			                if(i + j + k == h) {
			                    cube_SetXYZ(i, j, k, 1);
					    top_led_num++;
					    if(top_led_num >= top_sand_num) {
					        top_sand_num = 0;
						goto top_draw_finish1;
					    }
				        }
			            }
			        }
			    }
			}
top_draw_finish1:
		} else { //current_orient == UP2DOWN
			//draw the down part
    			for(h = -13;h <= -3; h++) {
    			    for(i = -1;i >= -8; i--) {
    			        for(j = -1;j >= -8; j--) {
    			            for(k = -1;k >= -8; k--) {
    			                if(i + j + k == h) {
    			                    cube_SetXYZ(i, j, k, 1);
					    bottom_led_num++;
					    if(bottom_led_num >= bottom_sand_num) {
					        bottom_led_num = 0;
						goto down_draw_finish2; 
					    }
					}
    			            }
    			        }
    			    }
    			}
down_draw_finish2:
			//draw the top part
			for(h = 0;h <= 10; h++){
			    for(i = 0;i <= 7; i++) {
			        for(j = 0;j <= 7; j++) {
			            for(k = 0;k <= 7; k++) {
			                if(i + j + k == h) {
			                    cube_SetXYZ(i, j, k, 1);
					    top_led_num++;
					    if(top_led_num >= top_sand_num) {
					        top_sand_num = 0;
						goto top_draw_finish2;
					    }
				        }
			            }
			        }
			    }
			}
top_draw_finish2:
		}
	}

	if(current_orient == UP2DOWN) {
            for(i = 0;i <= 7; i++)
	        for(j = 0;j <= 7; j++)	
	    	    for(k = 0;k <= 7; k++){
	                if(i + j + k <= 10)
	    		hg_MoveSand(HG_TOP, HG_UP, i, j, k);
	    	    }
            for(i = -8;i <= -1; i++)
	        for(j = -8;j <= -1; j++)	
	    	    for(k = -8;k <= -1; k++){
	                if(i + j + k >= -13)
	    		hg_MoveSand(HG_BOTTOM, HG_DOWN, i, j, k);
	    	    }
	} else if(current_orient == DOWN2UP) {
            for(i = 7;i >= 0; i--)
	        for(j = 7;j >= 0; j--)	
	    	    for(k = 7;k >= 0; k--){
	                if(i + j + k <= 10)
	    		hg_MoveSand(HG_TOP, HG_DOWN, i, j, k);
	    	    }
            for(i = -1;i >= -8; i--)
	        for(j = -1;j >= -8; j--)	
	    	    for(k = -1;k >= -8; k--){
	                if(i + j + k >= -13)
	    		hg_MoveSand(HG_BOTTOM, HG_UP, i, j, k);
	    	    }
	}

	printf("cnt:%d\n", cnt++);
	top_sand_num = 0;
	bottom_sand_num = 0;

	if(cnt % 2 == 0) {
	    cube_SetXYZ(0, 0, 0, trigger_led_state);
            cube_SetXYZ(-1, -1, -1, !trigger_led_state);
	}

	//store the current orientation
	previous_orient = current_orient;
    }
}
