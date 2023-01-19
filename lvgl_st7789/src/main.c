/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include "buttons.h"
// #include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#include "ui.h"

static uint32_t count;

static lv_group_t *input_group;
static lv_indev_drv_t enc_drv;
static lv_indev_t *enc_indev;

static bool buttons_allocated = false;
static buttonId_t last_pressed;

static void onButtonPressCb(buttonPressType_t type, buttonId_t id);

static void enocoder_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    if (!buttons_allocated) {
        return;
    }
    if (button_read(BUTTON_1)) {
        data->key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = BUTTON_1;
    } else if (button_read(BUTTON_2)) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = BUTTON_2;
    } else if (button_read(BUTTON_3)) {
        data->key = LV_KEY_LEFT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = BUTTON_3;
    } else {
        if (last_pressed == 0xFF) {
            return;
        }
        data->state = LV_INDEV_STATE_REL;
        switch (last_pressed) {
            case BUTTON_1:
                data->key = LV_KEY_RIGHT;
                break;
            case BUTTON_2:
                data->key = LV_KEY_ENTER;
                break;
            case BUTTON_3:
                data->key = LV_KEY_LEFT;
                break;
        }
        last_pressed = 0xFF;
    }
}

static void onButtonPressCb(buttonPressType_t type, buttonId_t id)
{
    LOG_DBG("Pressed %d, type: %d", id, type);

    // // Always allow force restart
    // if (type == BUTTONS_LONG_PRESS && id == BUTTON_3) {
        
    // }

    // if (type == BUTTONS_LONG_PRESS && id == BUTTON_2) {
    //     // TODO doesn't work, as this press is read later with lvgl and causes extra press in settings.
    //     // To fix each application must have exit button, maybe we can register long press on the whole view to exit
    //     // apps without input device
    //     //application_manager_exit_app();
    //     //return;
    // }

    // if (buttons_allocated) {
    //     // Handled by LVGL
    //     return;
    // }

    // if (type == BUTTONS_SHORT_PRESS) {
    //     if (id == BUTTON_3) {
    //         LOG_DBG("Close Watchface, open App Manager");
    //     } else if (id == BUTTON_2) {
    //         LOG_DBG("CloseWatchface, open Notifications page");
    //     } else {
    //         LOG_WRN("Unhandled button %d, type: %d, watch_state: %d", id, type, watch_state);
    //     }
    // } else {
    //     if (id == BUTTON_3) {
            
    //     } else {
    //         LOG_WRN("Unhandled button %d, type: %d, watch_state: %d", id, type, watch_state);
    //     }
    // }
}

void main(void)
{
	int err;
	char count_str[11] = {0};
	const struct device *display_dev;

	printk("Hello World! %s\n", CONFIG_BOARD);

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}	

	buttonsInit(&onButtonPressCb);

	lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = enocoder_read;
    enc_indev = lv_indev_drv_register(&enc_drv);

    input_group = lv_group_create();
    lv_group_set_default(input_group);
    lv_indev_set_group(enc_indev, input_group);

	ui_init();
	lv_task_handler();
	display_blanking_off(display_dev);

	while (1) {
		if ((count % 100) == 0U) {
			// sprintf(count_str, "%d", count/100U);
			// lv_label_set_text(count_label, count_str);
			lv_arc_set_value(ui_Screen1_Arc1, count/100U);
			lv_arc_set_value(ui_Screen1_Arc2, count/100U);
			lv_bar_set_value(ui_Screen1_Bar1, count/100U, LV_ANIM_OFF);
		}
		lv_task_handler();
		++count;
		k_sleep(K_MSEC(10));
	}
}
