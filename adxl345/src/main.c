/*
 * Copyright (c) 2019 Brett Witherspoon
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdio.h>
#include <device.h>
#include <drivers/sensor.h>

struct adxl345_data
{
    /* data */
    int16_t x;
    int16_t y;
    int16_t z;
};

void main(void)
{
	struct sensor_value accel[3];
	struct adxl345_data adxl345_data;

	const struct device *dev = device_get_binding(DT_LABEL(DT_INST(0, adi_adxl345)));
	if (dev == NULL) {
		printf("Device get binding device\n");
		return;
	}

	while (true) {
		
		if (sensor_sample_fetch(dev) < 0) {
				printf("sensor_sample_fetch failed\n");
		}		

		sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);

		adxl345_data.x = (int16_t) sensor_value_to_double(&accel[0]);
		adxl345_data.y = (int16_t) sensor_value_to_double(&accel[1]);
		adxl345_data.z = (int16_t) sensor_value_to_double(&accel[2]);

		printf("X:%d,Y:%d,Z:%d\r\n", adxl345_data.x, adxl345_data.y, adxl345_data.z); 

		k_sleep(K_MSEC(1000));	
	}
}
