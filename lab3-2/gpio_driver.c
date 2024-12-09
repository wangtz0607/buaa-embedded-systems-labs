#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/printk.h>

#define INPUT_PIN 36
#define OUTPUT_PIN 24

static int irq;
static int count;

static irqreturn_t gpio_irq_handler(int irq, void *dev_id) {
    ++count;
    printk(KERN_INFO "count=%d\n", count);
    int ret;
    if ((ret = gpio_request(OUTPUT_PIN, "example_output")) < 0) {
        printk(KERN_ERR "gpio_request: %d\n", ret);
        goto cleanup1;
    }
    if ((ret = gpio_direction_output(OUTPUT_PIN, 0)) < 0) {
        printk(KERN_ERR "gpio_direction_output: %d\n", ret);
        goto cleanup2;
    }
    gpio_set_value(OUTPUT_PIN, count % 2);
cleanup2:
    gpio_free(OUTPUT_PIN);
cleanup1:
    return IRQ_HANDLED;
}

static int __init gpio_driver_init(void) {
    int ret;
    if ((ret = gpio_request(INPUT_PIN, "example_input")) < 0) {
        printk(KERN_ERR "gpio_request: %d\n", ret);
        goto cleanup1;
    }
    if ((ret = gpio_direction_input(INPUT_PIN)) < 0) {
        printk(KERN_ERR "gpio_direction_input: %d\n", ret);
        goto cleanup2;
    }
    if ((ret = gpio_to_irq(INPUT_PIN)) < 0) {
        printk(KERN_ERR "gpio_to_irq: %d\n",  ret);
        goto cleanup2;
    }
    irq = ret;
    if ((ret = request_irq(irq, gpio_irq_handler, IRQF_TRIGGER_RISING, "example_input", NULL)) < 0) {
        printk(KERN_ERR "request_irq: %d\n", ret);
        goto cleanup2;
    }
    return 0;
cleanup2:
    gpio_free(INPUT_PIN);
cleanup1:
    return ret;
}

static void __exit gpio_driver_exit(void) {
    free_irq(irq, NULL);
    gpio_free(INPUT_PIN);
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
