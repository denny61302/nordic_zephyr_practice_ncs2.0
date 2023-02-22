/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/uart.h>
#include <lvgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "api.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define STACK_SIZE 8192
#define PRIORITY 7

K_THREAD_STACK_DEFINE(stack_area1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);

struct DATAMQ {  
  int data;
};

K_MSGQ_DEFINE(data_msgq, sizeof(struct DATAMQ), 80, 16);

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart1));

lv_obj_t *count_label;
lv_obj_t * meter;
lv_meter_scale_t * scale;
lv_meter_indicator_t * indic;
lv_obj_t * my_Cir;

void peripheral_callback(const struct device *dev, char *data, size_t length, bool is_string, void *user_data)
{
    struct DATAMQ data_msg;

    data_msg.data = atoi(data);

    printk("Time %d, %d\r\n", k_uptime_get_32(), data_msg.data);

    while (k_msgq_put(&data_msgq, &data_msg, K_NO_WAIT) != 0) {
              /* message queue is full: purge old data & try again */
              k_msgq_purge(&data_msgq);
          }
          // lv_tick_inc(10);
}

// Task: command line interface (CLI)
void doCLI() 
{
  const struct device *dev = device_get_binding(DT_LABEL(DT_NODELABEL(my_device)));
  my_uart_set_callback(dev, peripheral_callback, NULL);
}

void update_GUI(void)
{
	char count_str[11] = {0};
	struct DATAMQ data_msg;
  lv_area_t a;

	while (1) {
		// printk("Update GUI\r\n");
		k_msgq_get(&data_msgq, &data_msg, K_NO_WAIT);
		

		sprintf(count_str, "%d", data_msg.data);
		lv_label_set_text(count_label, count_str);
    lv_obj_set_size(my_Cir , 50, data_msg.data - 1900);
    // lv_meter_set_indicator_value(meter, indic, data_msg.data);
		
		lv_task_handler();
		k_yield();
	}
}

void main(void)
{
	int err;
	char count_str[11] = {0};
	const struct device *display_dev;

	if (!device_is_ready(uart_dev)) {
		LOG_ERR("UART device not ready\n");
		return;
	}


	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

	lv_disp_t * dispp = lv_disp_get_default();
  lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                              true, LV_FONT_DEFAULT);
  lv_disp_set_theme(dispp, theme);
	
  my_Cir = lv_obj_create(lv_scr_act());
  lv_obj_set_size(my_Cir , 0, 0);
  lv_obj_align(my_Cir, LV_ALIGN_CENTER, 75, 0);
  lv_obj_set_style_bg_color(my_Cir , lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_set_style_radius(my_Cir , LV_RADIUS_CIRCLE, 0);

	meter = lv_meter_create(lv_scr_act());
  lv_obj_align(meter, LV_ALIGN_CENTER, -75, 0);
  lv_obj_set_size(meter, 150, 150);

  /*Add a scale first*/
  scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
  lv_meter_set_scale_major_ticks(meter, scale, 8, 4, 15, lv_color_black(), 10);

  /*Add a blue arc to the start*/
  indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_meter_set_indicator_start_value(meter, indic, 0);
  lv_meter_set_indicator_end_value(meter, indic, 20);

  /*Make the tick lines blue at the start of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
  lv_meter_set_indicator_start_value(meter, indic, 0);
  lv_meter_set_indicator_end_value(meter, indic, 20);

  /*Add a red arc to the end*/
  indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
  lv_meter_set_indicator_start_value(meter, indic, 80);
  lv_meter_set_indicator_end_value(meter, indic, 100);

  /*Make the tick lines red at the end of the scale*/
  indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
  lv_meter_set_indicator_start_value(meter, indic, 80);
  lv_meter_set_indicator_end_value(meter, indic, 100);

  /*Add a needle line indicator*/
  indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

	count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	struct k_thread thread1, thread2;

	k_thread_create(&thread1, stack_area1,
                                 K_THREAD_STACK_SIZEOF(stack_area1),
                                 update_GUI,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&thread2, stack_area2,
                                 K_THREAD_STACK_SIZEOF(stack_area2),
                                 doCLI,
                                 NULL, NULL, NULL,
                                 4, 0, K_NO_WAIT);

}
