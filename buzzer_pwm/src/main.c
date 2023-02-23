/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#define MIN_PERIOD 7812500
#define MAX_PERIOD 250000000

#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494

#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

const int happy_birthday[] = {
	NOTE_C4,	NOTE_C4, 	NOTE_D4,	NOTE_C4,	NOTE_F4,
	NOTE_E4,	NOTE_C4,	NOTE_C4,	NOTE_D4,	NOTE_C4,
	NOTE_G4,	NOTE_F4,	NOTE_C4,	NOTE_C4,	NOTE_C5,	
	NOTE_A4,	NOTE_F4,	NOTE_E4,	NOTE_D4,	NOTE_AS4,	
	NOTE_AS4,	NOTE_A4,	NOTE_F4,	NOTE_G4,	NOTE_F4,	
};

const float happy_birthday_duration[25][2] = {
	{NOTE_C4,4},{NOTE_C4,8},{NOTE_D4,1.5},{NOTE_C4,1},{NOTE_F4,1},
	{NOTE_E4,3},{NOTE_C4,4},{NOTE_C4,8},{NOTE_D4,1.5},{NOTE_C4,1},
	{NOTE_G4,1},{NOTE_F4,3},{NOTE_C4,4},{NOTE_C4,8},{NOTE_C5,1.5},
	{NOTE_A4,1},{NOTE_F4,1},{NOTE_E4,1},{NOTE_D4,1},{NOTE_AS4,4},
	{NOTE_AS4,8},{NOTE_A4,1.5},{NOTE_F4,1},{NOTE_G4,1},{NOTE_F4,3}
};

void main(void)
{
	uint32_t max_period;
	uint32_t period;
	uint8_t dir = 0U;
	int ret;

	printk("PWM-based blinky\n");

	if (!device_is_ready(pwm_led0.dev)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led0.dev->name);
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

	printk("Done calibrating; maximum/minimum periods %u/%lu usec\n",
	       max_period, MIN_PERIOD);

	period = max_period;

	int freq = 0;
	float input_period = (float)(1.0 / freq) * 1000000000.0;
	int i=0;
	while (1) {
		freq = happy_birthday_duration[i][0];
		input_period = (float)(1.0 / freq) * 1000000000.0;
		ret = pwm_set_dt(&pwm_led0, input_period, input_period/2);
		i++;
		if (i==25) i=0;
		// if (ret) {
		// 	printk("Error %d: failed to set pulse width\n", ret);
		// 	return;
		// }

		// freq = dir ? (freq + 10) : (freq - 10);
		// if (freq > 1000) {
		// 	freq = freq - 10;
		// 	dir = 0U;
		// } else if (freq < 200) {
		// 	freq = freq + 10;
		// 	dir = 1U;
		// }		
		k_sleep(K_MSEC(happy_birthday_duration[i][1]*100));
	}

}
