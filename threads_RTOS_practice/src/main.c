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

#define LED0_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

struct led {
	struct gpio_dt_spec spec;
	const char *gpio_pin_name;
};

static const struct led led0 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0}),
	.gpio_pin_name = DT_PROP_OR(LED0_NODE, label, ""),
};


#define STACK_SIZE 1024
#define PRIORITY 7

K_THREAD_STACK_DEFINE(stack_area1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);

// Settings
static const uint8_t buf_len = 255;     // Size of buffer to look for command
static const char command[] = "delay "; // Note the space!

// Message struct: used to wrap strings (not necessary, but it's useful to see
// how to use structs here)

struct LEDMQ {  
  int led_delay;
};

K_MSGQ_DEFINE(delay_msgq, sizeof(struct LEDMQ), 20, 4);

static const struct device *uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

// Task: flash LED based on delay provided, notify other task every 100 blinks
void blinkLED() {

  struct LEDMQ led_msg;
  led_msg.led_delay = 500;

  // Loop forever
  while (1) {
    k_msgq_get(&delay_msgq, &led_msg, K_NO_WAIT);
    // Blink
    gpio_pin_toggle(led0.spec.port, led0.spec.pin);
    k_msleep(led_msg.led_delay);
  }
}

// Task: command line interface (CLI)
void doCLI() 
{
  char c;
  char buf[buf_len];
  uint8_t idx = 0;
  uint8_t cmd_len = strlen(command);
  struct LEDMQ led_msg;

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
        printk("update LED\r\n");
        // Convert last part to positive integer (negative int crashes)
        char* tail = buf + cmd_len;
        led_msg.led_delay = atoi(tail);
        led_msg.led_delay = abs(led_msg.led_delay);
        printk("Led delay: %d\r\n", led_msg.led_delay);
        // Send integer to other task via queue
        // k_queue_append(&delay_queue, &led_delay);
        /* send data to consumers */
        while (k_msgq_put(&delay_msgq, &led_msg, K_NO_WAIT) != 0) {
            /* message queue is full: purge old data & try again */
            k_msgq_purge(&delay_msgq);
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
                                 blinkLED,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&thread2, stack_area2,
                                 K_THREAD_STACK_SIZEOF(stack_area2),
                                 doCLI,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);
}