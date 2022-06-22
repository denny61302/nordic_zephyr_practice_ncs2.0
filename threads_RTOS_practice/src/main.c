/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <string.h>
#include <stdlib.h>
#include <drivers/uart.h>

#define STACK_SIZE 1024
#define PRIORITY 7

#define LED0_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

// Settings
static const uint8_t buf_len = 20;

// Globals
static int led_delay = 500;   // ms

K_THREAD_STACK_DEFINE(stack_area1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);

struct led {
	struct gpio_dt_spec spec;
	const char *gpio_pin_name;
};

static const struct led led0 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0}),
	.gpio_pin_name = DT_PROP_OR(LED0_NODE, label, ""),
};

static const struct device *uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

void toggleLED_1()
{	
	while (1) {
		gpio_pin_toggle(led0.spec.port, led0.spec.pin);
		// printk("rate1\r\n");
		k_msleep(led_delay);
	}
}

void readSerial() {

  unsigned char recv_char;
  char buf[buf_len];
  uint8_t idx = 0;

  // Clear whole buffer
  memset(buf, 0, buf_len);

  // Loop forever
  while (1) {
    // Read characters from serial
    while (uart_poll_in(uart_dev, &recv_char) < 0) {
			/* Allow other thread/workqueue to work. */
			k_yield();
	}

	// Update delay variable and reset buffer if we get a newline character
	if (recv_char == '\n') 
	{
		led_delay = atoi(buf);
		printk("Updated LED delay to: %d\n", led_delay);
		memset(buf, 0, buf_len);
		idx = 0;
	} 
	else 
	{        
		// Only append if index is not over message limit
		if (idx < buf_len - 1) 
		{
			buf[idx] = recv_char;
			idx++;
		}
	}    
  }
}

void main(void)
{
	int ret;

	if (!device_is_ready(uart_dev)) {
		printk("UART device not ready\n");
		return;
	}

	if (!device_is_ready(led0.spec.port)) {
		printk("Error: %s device is not ready\n", led0.spec.port->name);
		return;
	}

	ret = gpio_pin_configure_dt(&led0.spec, GPIO_OUTPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d (LED '%s')\n",
			ret, led0.spec.pin, led0.gpio_pin_name);
		return;
	}	

	struct k_thread thread1, thread2;

	k_thread_create(&thread1, stack_area1,
                                 K_THREAD_STACK_SIZEOF(stack_area1),
                                 toggleLED_1,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&thread2, stack_area2,
                                 K_THREAD_STACK_SIZEOF(stack_area2),
                                 readSerial,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);
}