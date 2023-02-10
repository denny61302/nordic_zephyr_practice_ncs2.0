/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
// #include <stdio.h>

static int counter = 0;
static bool size_dec = false;

static void repeating_timer_callback(struct k_work *dummy){
	char str[100];
	char null = '\0';
	if(size_dec) counter--;
    else counter++;

    if(counter == 100) size_dec = true;
    else if(counter == 0) size_dec = false;
	sprintf(str,"%d", counter);
	// printk("Timer counter %.2f\n", counter);
	printf(str);
	printf("%c", null);
}

K_WORK_DEFINE(repeating_timer_work, repeating_timer_callback);

void repeating_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&repeating_timer_work);
}

K_TIMER_DEFINE(my_timer, repeating_timer_handler, NULL);

void main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD);

	k_timer_init(&my_timer, repeating_timer_handler, NULL);
	k_timer_start(&my_timer, K_NO_WAIT, K_MSEC(100));	
}
