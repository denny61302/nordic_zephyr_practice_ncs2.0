/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>

#include "image.h"
LOG_MODULE_REGISTER(app);

LV_IMG_DECLARE(mdpp_logo);

void main(void)
{
	uint32_t count = 0U;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

	
	lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &mdpp_logo);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);	
	
	hello_world_label = lv_label_create(lv_scr_act());

	lv_label_set_text(hello_world_label, "#7400ff Hello MDPP!#");
	lv_label_set_recolor(hello_world_label,true);
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

	count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_MID, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d", count/100U);
			lv_label_set_text(count_label, count_str);
		}
		lv_task_handler();
		k_sleep(K_MSEC(10));
		++count;
	}
}
