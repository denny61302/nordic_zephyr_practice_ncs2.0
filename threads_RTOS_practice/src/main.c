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

#define STACK_SIZE 1024
#define PRIORITY 7

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED1_NODE, okay)
#error "Unsupported board: led1 devicetree alias is not defined"
#endif

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

void toggleLED_1()
{	
	while (1) {
		gpio_pin_toggle(led0.spec.port, led0.spec.pin);
		printk("rate1\r\n");
		k_msleep(500);
	}
}

void toggleLED_2()
{
	while (1) {
		gpio_pin_toggle(led0.spec.port, led0.spec.pin);
		printk("rate2\r\n");
		k_msleep(323);
	}
}

void main(void)
{
	int ret;

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
                                 toggleLED_2,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);
}

// K_THREAD_DEFINE(id1, STACK_SIZE, toggleLED_1, NULL, NULL, NULL,
// 		PRIORITY, 0, 0);

// K_THREAD_DEFINE(id2, STACK_SIZE, toggleLED_2, NULL, NULL, NULL,
// 		PRIORITY, 0, 0);

// struct printk_data_t {
// 	void *fifo_reserved; /* 1st word reserved for use by fifo */
// 	uint32_t led;
// 	uint32_t cnt;
// };

// K_FIFO_DEFINE(printk_fifo);

// struct led {
// 	struct gpio_dt_spec spec;
// 	const char *gpio_pin_name;
// };

// static const struct led led0 = {
// 	.spec = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0}),
// 	.gpio_pin_name = DT_PROP_OR(LED0_NODE, label, ""),
// };

// static const struct led led1 = {
// 	.spec = GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0}),
// 	.gpio_pin_name = DT_PROP_OR(LED1_NODE, label, ""),
// };

// void blink(const struct led *led, uint32_t sleep_ms, uint32_t id)
// {
// 	const struct gpio_dt_spec *spec = &led->spec;
// 	int cnt = 0;
// 	int ret;

// 	if (!device_is_ready(spec->port)) {
// 		printk("Error: %s device is not ready\n", spec->port->name);
// 		return;
// 	}

// 	ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
// 	if (ret != 0) {
// 		printk("Error %d: failed to configure pin %d (LED '%s')\n",
// 			ret, spec->pin, led->gpio_pin_name);
// 		return;
// 	}

// 	while (1) {
// 		gpio_pin_set(spec->port, spec->pin, cnt % 2);

// 		struct printk_data_t tx_data = { .led = id, .cnt = cnt };

// 		size_t size = sizeof(struct printk_data_t);
// 		char *mem_ptr = k_malloc(size);
// 		__ASSERT_NO_MSG(mem_ptr != 0);

// 		memcpy(mem_ptr, &tx_data, size);

// 		k_fifo_put(&printk_fifo, mem_ptr);

// 		k_msleep(sleep_ms);
// 		cnt++;
// 	}
// }

// void blink0(void)
// {
// 	blink(&led0, 100, 0);
// }

// void blink1(void)
// {
// 	blink(&led1, 1000, 1);
// }

// void uart_out(void)
// {
// 	while (1) {
// 		struct printk_data_t *rx_data = k_fifo_get(&printk_fifo,
// 							   K_FOREVER);
// 		printk("Toggled led%d; counter=%d\n",
// 		       rx_data->led, rx_data->cnt);
// 		k_free(rx_data);
// 	}
// }

// K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL,
// 		PRIORITY, 0, 0);
// K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
// 		PRIORITY, 0, 0);
// K_THREAD_DEFINE(uart_out_id, STACKSIZE, uart_out, NULL, NULL, NULL,
// 		PRIORITY, 0, 0);
