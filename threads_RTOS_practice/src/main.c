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

// Settings
static const uint8_t buf_len = 255;

// Globals
static char *msg_ptr = NULL;
static volatile uint8_t msg_flag = 0;

K_THREAD_STACK_DEFINE(stack_area1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_area2, STACK_SIZE);


static const struct device *uart_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

// Task: print message whenever flag is set and free buffer
void printMessage() {
  while (1) {

    // Wait for flag to be set and print message
    if (msg_flag == 1) {
      printk("%s", msg_ptr);

      // Give amount of free heap memory (uncomment if you'd like to see it)
     printk("Free heap (bytes): %d\r\n", );
//      printk(xPortGetFreeHeapSize());

      // Free buffer, set pointer to null, and clear flag
      k_free(msg_ptr);
      msg_ptr = NULL;
      msg_flag = 0;
    }
    else {
      k_yield();
    }
  }
}

void readSerial() 
{

  char recv_char;
  char buf[buf_len];
  uint8_t idx = 0;

  // Clear whole buffer
  memset(buf, 0, buf_len);



  // Loop forever
  while (1) {

    // Read cahracters from serial
    
      while (uart_poll_in(uart_dev, &recv_char) < 0) {
				/* Allow other thread/workqueue to work. */
				k_yield();
		}

      // Store received character to buffer if not over buffer limit
      if (idx < buf_len - 1) {
        buf[idx] = recv_char;
        idx++;
      }

      // Create a message buffer for print task
      if (recv_char == '\n') {

        // The last character in the string is '\n', so we need to replace
        // it with '\0' to make it null-terminated
        buf[idx - 1] = '\0';

        // Try to allocate memory and copy over message. If message buffer is
        // still in use, ignore the entire message.
        if (msg_flag == 0) {
          msg_ptr = (char *)k_malloc(idx * sizeof(char));

          // Copy message
          memcpy(msg_ptr, buf, idx);

          // Notify other task that message is ready
          msg_flag = 1;
        }

        // Reset receive buffer and index counter
        memset(buf, 0, buf_len);
        idx = 0;
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

	struct k_thread thread1, thread2;

	k_thread_create(&thread1, stack_area1,
                                 K_THREAD_STACK_SIZEOF(stack_area1),
                                 readSerial,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&thread2, stack_area2,
                                 K_THREAD_STACK_SIZEOF(stack_area2),
                                 printMessage,
                                 NULL, NULL, NULL,
                                 PRIORITY, 0, K_NO_WAIT);
}