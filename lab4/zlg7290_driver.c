#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define ZLG7290_NAME "zlg7290"
#define ZLG7290_LED_CLASS_NAME "zlg7290_led"
#define ZLG7290_LED_NAME "zlg7290_led"

#define REG_DP_RAM0 0x10
#define REG_DP_RAM1 0x11
#define REG_DP_RAM2 0x12
#define REG_DP_RAM3 0x13

#define ZLG7290_LED_MAJOR 800
#define ZLG7290_LED_MINOR 0
#define ZLG7290_LED_DEVICES 1

struct zlg7290 {
	struct i2c_client *client;
	struct input_dev *input;
	struct cdev cdev;
};

static struct zlg7290 *zlg7290;

static struct class *zlg7290_class = NULL;
static struct device *zlg7290_device = NULL;

static unsigned char bit_map[8] = {
	0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,
};

static unsigned char hex_map[16] = {
	0xfc, 0x60, 0xda, 0xf2, 0x66, 0xb6, 0xbe, 0xe4, 0xfe, 0xf6,
	0xee, 0x3e, 0x9c, 0x7a, 0x9e, 0x8e,
};

static int zlg7290_hw_write(struct zlg7290 *zlg7290, int len, char *buf) {
	struct i2c_client *client = zlg7290->client;
	struct i2c_msg msg[] = {
		{client->addr, 0, len, buf},
	};
	int ret;
	if ((ret = i2c_transfer(client->adapter, msg, 1)) < 0) {
		dev_err(&client->dev, "i2c_transfer: %d\n", ret);
		return -EIO;
	}
	return 0;
}

static ssize_t zlg7290_led_write(struct file *file, const char __user *buffer, size_t len, loff_t *offset) {
	char data[4];
    if (copy_from_user(data, buffer, 4)) {
        return -EFAULT;
    }
	int i;
	for (i = 0; i < 4; ++i) {
		char i2c_data[2];
		i2c_data[0] = REG_DP_RAM0 + i;
		switch (data[i]) {
			case '0': i2c_data[1] = hex_map[0]; break;
			case '1': i2c_data[1] = hex_map[1]; break;
			case '2': i2c_data[1] = hex_map[2]; break;
			case '3': i2c_data[1] = hex_map[3]; break;
			case '4': i2c_data[1] = hex_map[4]; break;
			case '5': i2c_data[1] = hex_map[5]; break;
			case '6': i2c_data[1] = hex_map[6]; break;
			case '7': i2c_data[1] = hex_map[7]; break;
			case '8': i2c_data[1] = hex_map[8]; break;
			case '9': i2c_data[1] = hex_map[9]; break;
			case 'a': case 'A': i2c_data[1] = hex_map[10]; break;
			case 'b': case 'B': i2c_data[1] = hex_map[11]; break;
			case 'c': case 'C': i2c_data[1] = hex_map[12]; break;
			case 'd': case 'D': i2c_data[1] = hex_map[13]; break;
			case 'e': case 'E': i2c_data[1] = hex_map[14]; break;
			case 'f': case 'F': i2c_data[1] = hex_map[15]; break;
			default: i2c_data[1] = 0x00; break;
		}
		int ret;
		if ((ret = zlg7290_hw_write(zlg7290, 2, i2c_data)) < 0) {
			return ret;
		}
	}
    return len;
}

static struct file_operations zlg7290_led_fops = {
	.owner = THIS_MODULE,
	.write = zlg7290_led_write,
};

static int register_zlg7290_led(struct zlg7290 *zlg7290) {
	int ret;
	struct cdev *zlg7290_cdev;
	dev_t dev_id = MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR);
	if ((ret = register_chrdev_region(dev_id, ZLG7290_LED_DEVICES, ZLG7290_LED_NAME)) < 0) {
		dev_err(&zlg7290->client->dev, "register_chrdev_region: %d\n", ret);
		goto cleanup1;
	}
	zlg7290_cdev = &zlg7290->cdev;
	cdev_init(zlg7290_cdev, &zlg7290_led_fops);
	zlg7290_cdev->owner = THIS_MODULE;
	if ((ret = cdev_add(zlg7290_cdev, dev_id, 1)) < 0) {
		dev_err(&zlg7290->client->dev, "cdev_add: %d\n", ret);
		goto cleanup2;
	}
	zlg7290_class = class_create(THIS_MODULE, ZLG7290_LED_CLASS_NAME);
	if (IS_ERR(zlg7290_class)) {
		ret = PTR_ERR(zlg7290_class);
		goto cleanup2;
	}
	zlg7290_device = device_create(zlg7290_class, NULL, dev_id, NULL, ZLG7290_LED_NAME);
	if (IS_ERR(zlg7290_device)) {
		ret = PTR_ERR(zlg7290_device);
		goto cleanup3;
	}
	return 0;

cleanup3:
	class_destroy(zlg7290_class);
cleanup2:
	unregister_chrdev_region(dev_id, ZLG7290_LED_DEVICES);
cleanup1:
	return ret;
}

static int unregister_zlg7290_led(struct zlg7290 *zlg7290) {
	device_destroy(zlg7290_class, MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR));
	class_destroy(zlg7290_class);
	cdev_del(&zlg7290->cdev);
	unregister_chrdev_region(MKDEV(ZLG7290_LED_MAJOR, ZLG7290_LED_MINOR), ZLG7290_LED_DEVICES);
	return 0;
}

static int zlg7290_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	printk(KERN_INFO "zlg7290_probe\n");
	struct input_dev *input_dev;
	int ret = 0;
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE)) {
		dev_err(&client->dev, "%s adapter not supported\n", dev_driver_string(&client->adapter->dev));
		ret = -ENODEV;
		goto cleanup1;
	}
	zlg7290 = kzalloc(sizeof(struct zlg7290), GFP_KERNEL);
    if (!zlg7290) {
        ret = -ENOMEM;
        goto cleanup1;
    }
	input_dev = input_allocate_device();
	if (!input_dev) {
		ret = -ENOMEM;
		goto cleanup2;
	}
	input_dev->name = client->name;
	input_dev->phys = "zlg7290-keys/input0";
	input_dev->dev.parent = &client->dev;
	input_dev->id.bustype = BUS_I2C;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0001;
	input_dev->evbit[0] = BIT_MASK(EV_KEY);
	if ((ret = input_register_device(input_dev)) < 0) {
		dev_err(&client->dev, "input_register_device: %d\n", ret);
		goto cleanup3;
	}
	zlg7290->client = client;
	zlg7290->input = input_dev;
	i2c_set_clientdata(client, zlg7290);	
	if ((ret = register_zlg7290_led(zlg7290)) < 0) {
		dev_err(&client->dev, "register_zlg7290_led: %d\n", ret);
		goto cleanup4;
	}
	return 0;

cleanup4:
	input_unregister_device(input_dev);
cleanup3:
	input_free_device(input_dev);
cleanup2:
	kfree(zlg7290);
cleanup1:
	return ret;
}

static int zlg7290_remove(struct i2c_client *client) {
	printk(KERN_INFO "zlg7290_remove\n");
	struct zlg7290 *zlg7290 = i2c_get_clientdata(client);
    unregister_zlg7290_led(zlg7290);
	return 0;
}

static const struct i2c_device_id zlg7290_device_id[] = {
	{ZLG7290_NAME, 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, zlg7290_device_id);

static const struct of_device_id zlg7290_dt_ids[] = {
	{.compatible = "myzr,zlg7290"},
	{},
};
MODULE_DEVICE_TABLE(of, zlg7290_dt_ids);

static struct i2c_driver zlg7290_driver = {
	.driver	= {
		.name = ZLG7290_NAME,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(zlg7290_dt_ids),
	},
	.probe = zlg7290_probe,
    .remove = zlg7290_remove,
	.id_table = zlg7290_device_id,
};

static int __init zlg7290_driver_init(void) {
	printk(KERN_INFO "zlg7290_driver_init\n");
	int i, j;
	unsigned char buf[2] = {0};
	i2c_register_driver(THIS_MODULE, &zlg7290_driver);
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 4; ++j){
			buf[0] = REG_DP_RAM0 + j;
			buf[1] = bit_map[i];
			zlg7290_hw_write(zlg7290, 2, buf);
			msleep(1);
		}
		msleep(200);
	}
    for (i = 0; i < 16; ++i) {
		for (j = 0; j < 4; ++j) {
			buf[0] = REG_DP_RAM0 + j;
			buf[1] = hex_map[i];
			zlg7290_hw_write(zlg7290, 2, buf);
			msleep(1);
		}
		msleep(200);
	}
    for (j = 0; j < 4; ++j) {
		buf[0] = REG_DP_RAM0 + j;
		buf[1] = 0x00;
		zlg7290_hw_write(zlg7290, 2, buf);
		msleep(1);
	}
	return 0;
}

static void __exit zlg7290_driver_exit(void) {
	printk(KERN_INFO "zlg7290_driver_exit\n");
    i2c_del_driver(&zlg7290_driver);
}

module_init(zlg7290_driver_init);
module_exit(zlg7290_driver_exit);

MODULE_LICENSE("GPL");
