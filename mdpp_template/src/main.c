#include <zephyr.h>

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <devicetree.h>
#include <dk_buttons_and_leds.h>
#include <drivers/sensor.h>
#if !DT_HAS_COMPAT_STATUS_OKAY(adi_adxl345)
#error "No adi,adxl345 compatible node found in the device tree"
#endif

#include <usb/usb_device.h>
#include <drivers/uart.h>

struct adxl345_data
{
    /* data */
    int16_t x;
    int16_t y;
    int16_t z;
};

#include "remote.h"

#define RUN_STATUS_LED DK_LED1
#define CONN_STATUS_LED DK_LED2
#define RUN_LED_BLINK_INTERVAL 250

static struct bt_conn *current_conn;
bool isNotify = false;
bool isConnected = false;
static int counter = 0;

// static void repeating_timer_callback(struct k_work *dummy){
// 	counter++;
// 	printk("Timer counter %d\n", counter);
// }

// K_WORK_DEFINE(repeating_timer_work, repeating_timer_callback);

void repeating_timer_handler(struct k_timer *dummy)
{
    // k_work_submit(&repeating_timer_work);
	counter++;
}

K_TIMER_DEFINE(my_timer, repeating_timer_handler, NULL);

/* Declarations */
void on_connected(struct bt_conn *conn, uint8_t err);
void on_disconnected(struct bt_conn *conn, uint8_t reason);
void on_notif_changed(enum bt_button_notifications_enabled status);
void on_data_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len);

struct bt_conn_cb bluetooth_callbacks = {
	.connected 		= on_connected,
	.disconnected 	= on_disconnected,
};

struct bt_remote_service_cb remote_service_callbacks = {
	.notif_changed = on_notif_changed,
	.data_received = on_data_received,
};

/* Callback */
void on_connected(struct bt_conn *conn, uint8_t err)
{
	if(err) {
		printk("connection err: %d\n", err);
		return;
	}
	printk("Connected.\n");
	current_conn = bt_conn_ref(conn);
	dk_set_led_on(CONN_STATUS_LED);
    isConnected = true;
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason: %d)\n", reason);
	dk_set_led_off(CONN_STATUS_LED);
	if(current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}	
    isConnected = false;
}

void on_notif_changed(enum bt_button_notifications_enabled status)
{
    if (status == BT_BUTTON_NOTIFICATIONS_ENABLED) {
		isNotify = true;
        printk("Notifications enabled\n");
    } else {
		isNotify = false;
        printk("Notifications disabled\n");
    }
}

void on_data_received(struct bt_conn *conn, const uint8_t *const data, uint16_t len)
{
    uint8_t temp_str[len+1];
    memcpy(temp_str, data, len);
    temp_str[len] = 0x00;

    printk("Received data on conn %p. Len: %d\n", (void *)conn, len);
    printk("Data: %s\n", temp_str);
}

void button_handler(uint32_t button_state, uint32_t has_changed)
{
    int err;
	int button_pressed = 0;
	if (has_changed & button_state)
	{
		switch (has_changed)
		{
			case DK_BTN1_MSK:
				button_pressed = 1;
				break;
			case DK_BTN2_MSK:
				button_pressed = 2;
				break;
			case DK_BTN3_MSK:
				button_pressed = 3;
				break;
			case DK_BTN4_MSK:
				button_pressed = 4;
				break;
			default:
				break;
		}
        printk("Button %d pressed.\n", button_pressed);       
		set_button_value(button_pressed);
		err = send_button_notification(current_conn, &button_pressed, 1);
		if (err) {
            printk("Couldn't send notificaton. (err: %d)\n", err);
        }
    }
}

/* Configurations */
static void configure_dk_buttons_leds(void)
{
    int err;
    err = dk_leds_init();
    if (err) {
        printk("Couldn't init LEDS (err %d)\n", err);
    }
    err = dk_buttons_init(button_handler);
    if (err) {
        printk("Couldn't init buttons (err %d)\n", err);
    }
}

void main(void)
{
	int err;
    int blink_status = 0;

    uint32_t count = 0U;
	char count_str[20] = {0};
    char ble_status_str[20] = {0};
    char battery_status_str[20] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
    lv_obj_t *count_label;
    lv_obj_t *ble_status_label;
    lv_obj_t *battery_status_label;


	struct sensor_value accel[3], voltage;
	struct adxl345_data adxl345_data;

	const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t baudrate, dtr = 0;

	if (!device_is_ready(dev)) {
		printk("CDC ACM device not ready");
		return;
	}


	if (usb_enable(NULL)) {
		return;
	}

	/* Poll if the DTR flag was set */
	do {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		/* Give CPU resources to low priority threads. */		
	} while (!dtr);

	/* Wait 1 sec for the host to do all settings */
	k_busy_wait(1000000);

	err = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (err) {
		printk("Failed to get baudrate, ret code %d", err);
	} else {
		printk("Baudrate detected: %d\n", baudrate);
	}

	printk("Hello World! %s\n", CONFIG_BOARD);

	configure_dk_buttons_leds();

	err = bluetooth_init(&bluetooth_callbacks, &remote_service_callbacks);
    if (err) {
        printk("Couldn't initialize Bluetooth. err: %d\n", err);
		return;
    }

	const struct device *sensor = DEVICE_DT_GET(DT_INST(0, adi_adxl345));

	if (sensor == NULL || !device_is_ready(sensor)) {
		printk("Could not get %s device\n", DT_LABEL(DT_INST(0, adi_adxl345)));
		// return;
	}

    display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		printk("Device not ready, aborting test");
		return;
	}

    hello_world_label = lv_label_create(lv_scr_act());

    lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

    battery_status_label = lv_label_create(lv_scr_act());
	lv_obj_align(battery_status_label, LV_ALIGN_TOP_LEFT, 0, 20);

    ble_status_label = lv_label_create(lv_scr_act());
	lv_obj_align(ble_status_label, LV_ALIGN_TOP_LEFT, 0, 0);

	count_label = lv_label_create(lv_scr_act());
	lv_obj_align(count_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    display_blanking_off(display_dev);
    lv_task_handler();


	k_timer_init(&my_timer, repeating_timer_handler, NULL);
	k_timer_start(&my_timer, K_NO_WAIT, K_MSEC(250));	

	while (1) {		
		if(counter > 0){
			counter = 0;
			dk_set_led(RUN_STATUS_LED, (blink_status++)%2);		

			if (sensor_sample_fetch(sensor) < 0) {
				printk("sensor_sample_fetch failed\n");
			}		

			sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, accel);

			adxl345_data.x = (int16_t) sensor_value_to_double(&accel[0]);
			adxl345_data.y = (int16_t) sensor_value_to_double(&accel[1]);
			adxl345_data.z = (int16_t) sensor_value_to_double(&accel[2]);

			if(isConnected){
                sprintf(ble_status_str, "BLE: Connected");
            } else {
                sprintf(ble_status_str, "BLE: Disconnected");
            }

			if(isNotify){
                sprintf(ble_status_str, "BLE: Notified");
				err = send_adxl345_notification(current_conn, (uint8_t*)&adxl345_data, sizeof(adxl345_data));
				if (err) {
					printk("Couldn't send notificaton. (err: %d)\n", err);
				}
			}
            lv_task_handler();
            sprintf(count_str, "X:%d,Y:%d,Z:%d", adxl345_data.x, adxl345_data.y, adxl345_data.z);
			lv_label_set_text(count_label, count_str);
            lv_label_set_text(ble_status_label, ble_status_str);
            lv_label_set_text(battery_status_label, battery_status_str);
			printk("X:%d,Y:%d,Z:%d\r\n", adxl345_data.x, adxl345_data.y, adxl345_data.z); 
		}
		k_sleep(K_MSEC(1));		
	}	
}