#include "remote.h"

// #define LOG_MODULE_NAME remote
// LOG_MODULE_REGISTER(LOG_MODULE_NAME);

static K_SEM_DEFINE(bt_init_ok, 0, 1);

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME)-1)

static uint8_t button_value = 0;
static struct bt_remote_service_cb remote_service_callbacks;
enum bt_button_notifications_enabled notifications_enabled;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN)
};

static const struct bt_data sd[] = {
    // BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_REMOTE_SERV_VAL),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
		      0x84, 0xaa, 0x60, 0x74, 0x52, 0x8a, 0x8b, 0x86,
		      0xd3, 0x4c, 0xb7, 0x1d, 0x1d, 0xdc, 0x53, 0x8d),
};

/* Declarations */
static ssize_t read_button_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
void button_chrc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
void adxl345_chrc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value);
static ssize_t on_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);


BT_GATT_SERVICE_DEFINE(remote_srv,
BT_GATT_PRIMARY_SERVICE(BT_UUID_REMOTE_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_REMOTE_BUTTON_CHRC,
                    BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                    BT_GATT_PERM_READ,
                    read_button_characteristic_cb, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_ADXL345_CHRC,
                    BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ, NULL, NULL, NULL),
    BT_GATT_CCC(adxl345_chrc_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),  // notification CCC
    BT_GATT_CHARACTERISTIC(BT_UUID_REMOTE_MESSAGE_CHRC, 
                    BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                    BT_GATT_PERM_WRITE,
                    NULL, on_write, NULL), 
);

/* Callback */

void bt_ready(int err)
{
    if (err) {
        printk("bt_ready returned %d\n", err);
    }

    k_sem_give(&bt_init_ok);
}

// read characteristic callback
static ssize_t read_button_characteristic_cb(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &button_value,
				 sizeof(button_value));
}

// notification callback
// void button_chrc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
// {
//     bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);
//     LOG_INF("Notifications %s", notif_enabled? "enabled":"disabled");
//     notifications_enabled = notif_enabled ? BT_BUTTON_NOTIFICATIONS_ENABLED:BT_BUTTON_NOTIFICATIONS_DISABLED;

//     if (remote_service_callbacks.notif_changed) {
//         remote_service_callbacks.notif_changed(notifications_enabled);
//     }
// }

void adxl345_chrc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);
    printk("Notifications %s\n", notif_enabled? "enabled":"disabled");
    notifications_enabled = notif_enabled ? BT_BUTTON_NOTIFICATIONS_ENABLED:BT_BUTTON_NOTIFICATIONS_DISABLED;

    if (remote_service_callbacks.notif_changed) {
        remote_service_callbacks.notif_changed(notifications_enabled);
    }
}

void on_sent(struct bt_conn *conn, void *user_data)
{
    ARG_UNUSED(user_data);
    printk("Notification sent on connection %p\n", (void *)conn);
}

static ssize_t on_write(struct bt_conn *conn,
                        const struct bt_gatt_attr *attr,
                        const void *buf,
                        uint16_t len,
                        uint16_t offset,
                        uint8_t flags)
{
    printk("Received data, handle %d, conn %p\n",
        attr->handle, (void *)conn);

    if (remote_service_callbacks.data_received) {
        remote_service_callbacks.data_received(conn, buf, len);
    }
    return len;
}

/* Remote controller functions */

void set_button_value(uint8_t btn_value)
{
    button_value = btn_value;
}

int send_button_notification(struct bt_conn *conn, uint8_t *value, uint16_t length)
{
    int err = 0;

    struct bt_gatt_notify_params params = {0};
    const struct bt_gatt_attr *attr = &remote_srv.attrs[2];

    params.attr = attr;
    params.data = value;
    params.len = length;
    params.func = on_sent;

    err = bt_gatt_notify_cb(conn, &params);

    return err;
}

int send_adxl345_notification(struct bt_conn *conn, uint8_t *value, uint16_t length)
{
    int err = 0;

    struct bt_gatt_notify_params params = {0};
    const struct bt_gatt_attr *attr = &remote_srv.attrs[4];

    params.attr = attr;
    params.data = value;
    params.len = length;
    params.func = on_sent;

    err = bt_gatt_notify_cb(conn, &params);

    return err;
}

int bluetooth_init(struct bt_conn_cb *bt_cb, struct bt_remote_service_cb *remote_cb)
{
    int err;
    printk("Initializing bluetooth\n");

    if (bt_cb == NULL || remote_cb == NULL) {
        return NRFX_ERROR_NULL;
    }
    bt_conn_cb_register(bt_cb);
    printk("build time: " __DATE__ " " __TIME__ "\n");
    os_mgmt_register_group();
    img_mgmt_register_group();
    smp_bt_register();
    remote_service_callbacks.notif_changed = remote_cb->notif_changed;
    remote_service_callbacks.data_received = remote_cb->data_received;

    err = bt_enable(bt_ready);
    if (err) {
        printk("bt_enable returned %d\n", err);
        return err;
    }

    k_sem_take(&bt_init_ok, K_FOREVER);

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err){
        printk("couldn't start advertising (err = %d\n", err);
        return err;
    }

    return err;
}