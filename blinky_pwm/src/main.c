/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>
#include <drivers/gpio.h>

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));

#define MIN_PERIOD PWM_SEC(1U) / 512U
#define MAX_PERIOD PWM_SEC(1U)

void main(void)
{
	const struct device *dev;
	
	uint32_t max_period;
	uint32_t period;
	uint8_t dir = 0U;
	int ret;
	float factor=0;

	dev = device_get_binding("GPIO_0");
	if (dev == NULL) {
		return;
	}

	ret = gpio_pin_configure(dev, 26, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_LOW);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_configure(dev, 27, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_LOW);
	if (ret < 0) {
		return;
	}

	gpio_pin_set(dev,26,0);
	gpio_pin_set(dev,27,1);

	printk("PWM-based blinky\n");

	if (!device_is_ready(pwm_led0.dev)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led0.dev->name);
		return;
	}

	if (!device_is_ready(pwm_led1.dev)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led1.dev->name);
		return;
	}

	/*
	 * In case the default MAX_PERIOD value cannot be set for
	 * some PWM hardware, decrease its value until it can.
	 *
	 * Keep its value at least MIN_PERIOD * 4 to make sure
	 * the sample changes frequency at least once.
	 */
	printk("Calibrating for channel %d...\n", pwm_led0.channel);
	max_period = MAX_PERIOD;
	while (pwm_set_dt(&pwm_led0, max_period, max_period / 2U)) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
			       "does not support a period at least %lu\n",
			       4U * MIN_PERIOD);
			return;
		}
	}

	printk("Calibrating for channel %d...\n", pwm_led1.channel);
	max_period = MAX_PERIOD;
	while (pwm_set_dt(&pwm_led1, max_period, max_period / 2U)) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
			       "does not support a period at least %lu\n",
			       4U * MIN_PERIOD);
			return;
		}
	}

	printk("Done calibrating; maximum/minimum periods %u/%lu usec\n",
	       max_period, MIN_PERIOD);

	period = MIN_PERIOD;
	
	while (factor <= 10) {
		
		
		printf("factor %.2f\r\n", (float)(factor / 10));
		
		ret = pwm_set_dt(&pwm_led0, period, period * (factor / 10));
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return;
		}
		ret = pwm_set_dt(&pwm_led1, period, period * (factor / 10));
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return;
		}

		if(factor == 10) 
		{			
			dir = 1;
			gpio_pin_set(dev,26,1);
			gpio_pin_set(dev,27,0);
		} else if (factor == 0)
		{
			dir = 0;
			gpio_pin_set(dev,26,0);
			gpio_pin_set(dev,27,1);
		}
		if (dir == 0) {
			factor = factor + 1;
		}
		else {
			factor = factor - 1;
		}
		
		k_sleep(K_SECONDS(2U));
	}
}
