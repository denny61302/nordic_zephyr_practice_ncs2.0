#include <zephyr/zephyr.h>

#include <zephyr/device.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/devicetree.h>

#include "adxl345.h"

#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>

bool isDataReady = false;
struct adxl345_data adxl345_data;
K_SEM_DEFINE(sem, 0, 1);

static void repeating_timer_callback(struct k_work *dummy){
	
	int err = readXYZ(&adxl345_data);
	if(err != 0){
		printk("Failed to read adxl345 data");
	}
	isDataReady = false;
	k_sem_give(&sem);
}

K_WORK_DEFINE(repeating_timer_work, repeating_timer_callback);

void isr_fn(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	isDataReady = true;
	k_work_submit(&repeating_timer_work);	
	printk("ISR triggered at %" PRIu32 "\n", k_cycle_get_32());
}
/* STEP 3 - Define a variable of type static struct gpio_callback */
static struct gpio_callback cb_data;

void main(void)
{
	int err;	
	printk("Hello World! %s\n", CONFIG_BOARD);

	const struct device *gpio;

	gpio = device_get_binding("GPIO_1");
	if (gpio == NULL) {
		return;
	}

	err = gpio_pin_configure(gpio, 9, GPIO_INPUT);
    if (err < 0) {
        return;
    }
	/* STEP 4 - Configure the interrupt on the pin by calling the function gpio_pin_interrupt_configure()  */
    err = gpio_pin_interrupt_configure(gpio, 9, GPIO_INT_EDGE_TO_ACTIVE);	
	/* STEP 6 - Initialize the static struct gpio_callback variable   */
	gpio_init_callback(&cb_data, isr_fn, BIT(9));	
	/* STEP 7 - Add the callback function by calling gpio_add_callback()   */
	gpio_add_callback(gpio, &cb_data);	

	err = adxl345_init();
	if(err != 0){
        printk("Failed to init ADXL345");
    }	

	do{
		err = readXYZ(&adxl345_data);
		if(err != 0) {
			printk("Failed to read adxl345 data");
		}	
		// printk("X:%d,Y:%d,Z:%d\r\n", adxl345_data.x, adxl345_data.y, adxl345_data.z); 
	} while(!isDataReady);
	

	while (1) {			
		k_sem_take(&sem, K_FOREVER);		
		printk("X:%d,Y:%d,Z:%d\r\n", adxl345_data.x, adxl345_data.y, adxl345_data.z); 
	}	
}