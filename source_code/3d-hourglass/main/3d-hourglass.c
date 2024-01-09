#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

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
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    int cnt = 0;

    for(uint8_t i = 0;i < 8; ++i) {
	gpio_set_level(g_gpios[i], 1);
        gpio_set_level(l_gpios[i], 0);
        gpio_set_level(rd_gpios[i], 1);
    }
    gpio_set_level(g_gpios[7], 0);
    gpio_set_level(g_gpios[6], 0);
    
    gpio_set_level(l_gpios[0], 1);
    gpio_set_level(l_gpios[1], 1);
    gpio_set_level(l_gpios[2], 1);
    gpio_set_level(l_gpios[3], 1);
    gpio_set_level(l_gpios[4], 1);
    gpio_set_level(l_gpios[5], 1);
    gpio_set_level(l_gpios[6], 1);
    gpio_set_level(l_gpios[7], 1);

    gpio_set_level(rd_gpios[0], 1);
    gpio_set_level(rd_gpios[1], 1);
    gpio_set_level(rd_gpios[2], 1);
    gpio_set_level(rd_gpios[3], 0);
    gpio_set_level(rd_gpios[4], 1);
    gpio_set_level(rd_gpios[5], 1);
    gpio_set_level(rd_gpios[6], 1);
    gpio_set_level(rd_gpios[7], 1);

    while(1) {
	vTaskDelay(500 / portTICK_PERIOD_MS);
	printf("cnt:%d\n", cnt++);
    }
}
