#include "vl6180x.h"

const struct device *dev;

int vl6180x_init()
{
    dev = device_get_binding(I2C0);
	if (dev == NULL) {
		printk("Could not find  %s!\n\r",I2C0);
		return -1;
	}

    uint8_t ret;
    uint8_t buf[] = {(0x0000 >> 8), 0x0000 & 0xff};
    i2c_write_read(dev, VL6180X_DEFAULT_ADDRESS, &buf, 2, &ret, 1);
    printk("ID: %d\r\n", ret);

    ret = vl6180x_read_register(0x016);
    if (ret==1) {
        vl6180x_configure();
        vl6180x_write_register(0x016, 0x00);
    }

    return 0;
}

int VL6180_Poll_Range() {
    char status;
    char range_status;

    // check the status
    status = vl6180x_read_register(0x04f);
    range_status = status & 0x07;

    // wait for new measurement ready status
    while (range_status != 0x04) {
        status = vl6180x_read_register(0x04f);
        range_status = status & 0x07;
    }
    return 0;
}

int VL6180_Start_Range() {
    vl6180x_write_register(0x018,0x03);
    return 0;
}

int VL6180_Read_Range() {
    int range;
    range=vl6180x_read_register(0x062);
    return range;
}

int VL6180_Clear_Interrupts() {
    vl6180x_write_register(0x015,0x07);
    return 0;
}

uint8_t vl6180x_measure_distance(uint8_t *out_mm)
{
    uint8_t status, mm;

    /* Start measurement. */
    // VL6180_Start_Range();

    /* Wait for measurement ready. */
    VL6180_Poll_Range();

    /* Read result. */
    mm = VL6180_Read_Range();
    *out_mm = mm;

    /* Clear interrupt flags. */
    VL6180_Clear_Interrupts();

    /* Return error code. */
    return (status >> 4);
}

void vl6180x_configure()
{
    // while (vl6180x_read_register(0x16) != 0x01) {
    // }

    /* Mandatory: Private registers. */
    vl6180x_write_register(0x0207, 0x01);
    vl6180x_write_register(0x0208, 0x01);
    vl6180x_write_register(0x0096, 0x00);
    vl6180x_write_register(0x0097, 0xfd);
    vl6180x_write_register(0x00e3, 0x00);
    vl6180x_write_register(0x00e4, 0x04);
    vl6180x_write_register(0x00e5, 0x02);
    vl6180x_write_register(0x00e6, 0x01);
    vl6180x_write_register(0x00e7, 0x03);
    vl6180x_write_register(0x00f5, 0x02);
    vl6180x_write_register(0x00d9, 0x05);
    vl6180x_write_register(0x00db, 0xce);
    vl6180x_write_register(0x00dc, 0x03);
    vl6180x_write_register(0x00dd, 0xf8);
    vl6180x_write_register(0x009f, 0x00);
    vl6180x_write_register(0x00a3, 0x3c);
    vl6180x_write_register(0x00b7, 0x00);
    vl6180x_write_register(0x00bb, 0x3c);
    vl6180x_write_register(0x00b2, 0x09);
    vl6180x_write_register(0x00ca, 0x09);
    vl6180x_write_register(0x0198, 0x01);
    vl6180x_write_register(0x01b0, 0x17);
    vl6180x_write_register(0x01ad, 0x00);
    vl6180x_write_register(0x00ff, 0x05);
    vl6180x_write_register(0x0100, 0x05);
    vl6180x_write_register(0x0199, 0x05);
    vl6180x_write_register(0x01a6, 0x1b);
    vl6180x_write_register(0x01ac, 0x3e);
    vl6180x_write_register(0x01a7, 0x1f);
    vl6180x_write_register(0x0030, 0x00);

    /* Recommended : Public registers - See data sheet for more detail */

    /* Enables polling for New Sample ready when measurement completes */
    vl6180x_write_register(0x0011, 0x10);
    /* Set the averaging sample period (compromise between lower noise and
     * increased execution time) */
    vl6180x_write_register(0x010a, 0x30);
    /* Sets the light and dark gain (upper nibble). Dark gain should not be
     * changed.*/
    vl6180x_write_register(0x003f, 0x46);
    /* sets the # of range measurements after which auto calibration of system
     * is performed */
    vl6180x_write_register(0x0031, 0xFF);
    /* Set ALS integration time to 100ms */
    vl6180x_write_register(0x0041, 0x63);
    /* perform a single temperature calibration of the ranging sensor */
    vl6180x_write_register(0x002e, 0x01);
}


void vl6180x_write_register(uint16_t reg, uint8_t val)
{
    uint8_t buf[] = {(reg >> 8), reg & 0xff, val & 0xff};
    i2c_write_read(dev, VL6180X_DEFAULT_ADDRESS, &buf, 3, NULL, 0);
}

uint8_t vl6180x_read_register(uint16_t reg)
{
    uint8_t ret;
    uint8_t buf[] = {(reg >> 8), reg & 0xff};
    i2c_write_read(dev, VL6180X_DEFAULT_ADDRESS, &buf, 2, &ret, 1);
    return ret;
}