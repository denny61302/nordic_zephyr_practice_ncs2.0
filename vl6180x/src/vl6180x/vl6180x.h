#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/util.h>
#include <zephyr.h>
#include <drivers/i2c.h>   

/* The devicetree node identifier for the "i2c0" */
#define I2C0_NODE DT_NODELABEL(i2c0)
#if DT_NODE_HAS_STATUS(I2C0_NODE, okay)
#define I2C0	DT_LABEL(I2C0_NODE)
#else
/* A build error here means your board does not have I2C enabled. */
#error "i2c0 devicetree node is disabled"
#define I2C0	""
#endif

/* Default address of the sensor after powerup. */
#define VL6180X_DEFAULT_ADDRESS 0x29

#define VL6180X_REGISTERS_H

#define VL6180X_REGISTERS_HVL6180X_REGISTERS_H

#define VL6180X_SYSTEM_INTERRUPT_CLEAR 0x015

#define VL6180X_SYSRANGE_START 0x018

#define VL6180X_RESULT_RANGE_STATUS 0x04d
#define VL6180X_RESULT_INTERRUPT_STATUS_GPIO 0x04f
#define VL6180X_RESULT_RANGE_VAL 0x062


int vl6180x_init();
uint8_t vl6180x_read_register(uint16_t reg);
void vl6180x_write_register( uint16_t reg, uint8_t val);

/** Returns 0 if the distance was measured correctly, the error code otherwise. */
uint8_t vl6180x_measure_distance(uint8_t *out_mm);
int VL6180_Poll_Range();
int VL6180_Start_Range();
int VL6180_Read_Range();
int VL6180_Clear_Interrupts();

/** Sends initial configuration to device. */
void vl6180x_configure();

#ifdef __cplusplus
}
#endif
#endif