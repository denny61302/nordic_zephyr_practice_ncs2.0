/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#include <device.h>
#include <drivers/counter.h>
#include <sys/printk.h>

#define DELAY 1000000
#define ALARM_CHANNEL_ID 0

struct counter_alarm_cfg alarm_cfg;

#define TIMER DT_LABEL(DT_NODELABEL(rtc2))


static void test_counter_interrupt_fn(const struct device *counter_dev,
				      uint8_t chan_id, uint32_t ticks,
				      void *user_data)
{
	struct counter_alarm_cfg *config = user_data;
	uint32_t now_ticks;
	uint64_t now_usec;
	int now_sec;
	int err;

	err = counter_get_value(counter_dev, &now_ticks);
	if (err) {
		LOG_INF("Failed to read counter value (err %d)", err);
		return;
	}

	now_usec = counter_ticks_to_us(counter_dev, now_ticks);
	now_sec = (int)(now_usec / USEC_PER_SEC);

	LOG_INF("!!! Alarm !!!\n");
	LOG_INF("Now: %u\n", now_sec);

	/* Set a new alarm with a double length duration */
	// config->ticks = config->ticks * 2U;

	LOG_INF("Set alarm in %u sec (%u ticks)\n",
	       (uint32_t)(counter_ticks_to_us(counter_dev,
					   config->ticks) / USEC_PER_SEC),
	       config->ticks);

	err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID,
					user_data);
	if (err != 0) {
		LOG_INF("Alarm could not be set\n");
	}
}

void main(void)
{
	const struct device *counter_dev;
	int err;

	LOG_INF("Counter alarm sample\n\n");
	counter_dev = device_get_binding(TIMER);
	if (counter_dev == NULL) {
		LOG_INF("Device not found\n");
		return;
	}

	counter_start(counter_dev);

	err = counter_get_frequency(counter_dev);
	LOG_INF("Freq: %d\r\n", err);

	alarm_cfg.flags = 0;
	alarm_cfg.ticks = counter_us_to_ticks(counter_dev, DELAY);
	alarm_cfg.callback = test_counter_interrupt_fn;
	alarm_cfg.user_data = &alarm_cfg;

	err = counter_set_channel_alarm(counter_dev, ALARM_CHANNEL_ID,
					&alarm_cfg);
	LOG_INF("Set alarm in %u sec (%u ticks)\n",
	       (uint32_t)(counter_ticks_to_us(counter_dev,
					   alarm_cfg.ticks) / USEC_PER_SEC),
	       alarm_cfg.ticks);

	if (-EINVAL == err) {
		LOG_INF("Alarm settings invalid\n");
	} else if (-ENOTSUP == err) {
		LOG_INF("Alarm setting request not supported\n");
	} else if (err != 0) {
		LOG_INF("Error\n");
	}

	while (1) {
		k_sleep(K_FOREVER);
	}
}
