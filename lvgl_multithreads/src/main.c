/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <lvgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#define STACK_SIZE 1024
#define PRIORITY 7

K_THREAD_STACK_DEFINE(stack_area1, 4096);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);


// Settings
static const uint8_t buf_len = 255;     // Size of buffer to look for command
static const char command[] = "data "; // Note the space!

struct DATAMQ {  
  int data;
};

K_MSGQ_DEFINE(data_msgq, sizeof(struct DATAMQ), 20, 4);

static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart1));

static uint32_t count;

lv_obj_t *count_label;
lv_obj_t *hello_world_label;

#ifdef CONFIG_GPIO
static struct gpio_dt_spec button_gpio = GPIO_DT_SPEC_GET_OR(
		DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_callback;

static void button_isr_callback(const struct device *port,
				struct gpio_callback *cb,
				uint32_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	count = 0;
}
#endif


// Task: flash LED based on delay provided, notify other task every 100 blinks
void readData() {

  struct DATAMQ data_msg;

  // Loop forever
  while (1) {
    k_msgq_get(&data_msgq, &data_msg, K_NO_WAIT);
    k_sleep(K_MSEC(10));
  }
}

// Task: command line interface (CLI)
void doCLI() 
{
  char c;
  char buf[buf_len];
  uint8_t idx = 0;
  uint8_t cmd_len = strlen(command);
  struct DATAMQ data_msg;

  // Clear whole buffer
  memset(buf, 0, buf_len);

  // Loop forever
  while (1) {

    // Read cahracters from serial
    
    while (uart_poll_in(uart_dev, &c) < 0) {
        /* Allow other thread/workqueue to work. */
        k_yield();
    }

    // Store received character to buffer if not over buffer limit
    if (idx < buf_len - 1) {
      buf[idx] = c;
      idx++;
    }

    // Print newline and check input on 'enter'
    if ((c == '\n') || (c == '\r')) {

      // Print newline to terminal
      printk("\r\n");
      printk("%s", buf);

      // Check if the first 6 characters are "delay "
      if (memcmp(buf, command, cmd_len) == 0) {
        printk("update data\r\n");
        // Convert last part to positive integer (negative int crashes)
        char* tail = buf + cmd_len;
        data_msg.data = atoi(tail);
        data_msg.data = abs(data_msg.data);
        printk("New data: %d\r\n", data_msg.data);
        // Send integer to other task via queue
        // k_queue_append(&delay_queue, &led_delay);
        /* send data to consumers */
        while (k_msgq_put(&data_msgq, &data_msg, K_NO_WAIT) != 0) {
            /* message queue is full: purge old data & try again */
            k_msgq_purge(&data_msgq);
        }
      }

      // Reset receive buffer and index counter
      memset(buf, 0, buf_len);
      idx = 0;

    // Otherwise, echo character back to serial terminal
    } else {
      printk("%c\r\n",c);
    }
  }  
}

void update_GUI(void)
{
	char count_str[11] = {0};
	struct DATAMQ data_msg;

	while (1) {
		printk("Update GUI\r\n");
		k_msgq_get(&data_msgq, &data_msg, K_NO_WAIT);

		sprintf(count_str, "%d", data_msg.data);
		lv_label_set_text(count_label, count_str);
		
		lv_task_handler();
		k_sleep(K_MSEC(30));
	}
}

void main(void)
{
	int err;
	char count_str[11] = {0};
	const struct device *display_dev;

	if (!device_is_ready(uart_dev)) {
		printk("UART device not ready\n");
		return;
	}


	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

#ifdef CONFIG_GPIO
	if (device_is_ready(button_gpio.port)) {
		err = gpio_pin_configure_dt(&button_gpio, GPIO_INPUT);
		if (err) {
			LOG_ERR("failed to configure button gpio: %d", err);
			return;
		}

		gpio_init_callback(&button_callback, button_isr_callback,
				   BIT(button_gpio.pin));

		err = gpio_add_callback(button_gpio.port, &button_callback);
		if (err) {
			LOG_ERR("failed to add button callback: %d", err);
			return;
		}

		err = gpio_pin_interrupt_configure_dt(&button_gpio,
						      GPIO_INT_EDGE_TO_ACTIVE);
		if (err) {
			LOG_ERR("failed to enable button callback: %d", err);
			return;
		}
	}
#endif

	if (IS_ENABLED(CONFIG_LV_Z_POINTER_KSCAN)) {
		lv_obj_t *hello_world_button;

		hello_world_button = lv_btn_create(lv_scr_act());
		lv_obj_align(hello_world_button, LV_ALIGN_CENTER, 0, 0);
		hello_world_label = lv_label_create(hello_world_button);
	} else {
		hello_world_label = lv_label_create(lv_scr_act());
	}

	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

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
                                 PRIORITY, 0, K_NO_WAIT);

	// k_thread_create(&thread3, stack_area3,
    //                              K_THREAD_STACK_SIZEOF(stack_area3),
    //                              update_GUI,
    //                              NULL, NULL, NULL,
    //                              PRIORITY, 0, K_NO_WAIT);

	// while (1) {
	// 	if ((count % 100) == 0U) {
	// 		sprintf(count_str, "%d", count/100U);
	// 		lv_label_set_text(count_label, count_str);
	// 	}
	// 	lv_task_handler();
	// 	++count;
	// 	k_sleep(K_MSEC(10));
	// }
}
