/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

/* STEP 9 - Increase the main thread sleep time from 100ms to 10 minutes  */
#define SLEEP_TIME_MS   1000*60*10

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	gpio_pin_toggle(dev,13);
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}
/* STEP 3 - Define a variable of type static struct gpio_callback */
static struct gpio_callback button_cb_data;

void main(void)
{
	const struct device *dev;
	int ret;

	dev = device_get_binding("GPIO_0");
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, 13, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_LOW);
	if (ret < 0) {
		return;
	}
    ret = gpio_pin_configure(dev, 11, GPIO_INPUT | GPIO_PULL_UP | GPIO_ACTIVE_LOW);
    if (ret < 0) {
        return;
    }
	/* STEP 4 - Configure the interrupt on the pin by calling the function gpio_pin_interrupt_configure()  */
    ret = gpio_pin_interrupt_configure(dev,
                       11,
                       GPIO_INT_EDGE_TO_ACTIVE | GPIO_INT_DEBOUNCE);	
	/* STEP 6 - Initialize the static struct gpio_callback variable   */
	gpio_init_callback(&button_cb_data, button_pressed, BIT(11));	
	/* STEP 7 - Add the callback function by calling gpio_add_callback()   */
	gpio_add_callback(dev, &button_cb_data);	
	while (1) {
		/* STEP 8 - Remove the polling code and put the main thread to sleep */
		// gpio_pin_toggle(dev, 13);
        k_msleep(SLEEP_TIME_MS); 
	}
}