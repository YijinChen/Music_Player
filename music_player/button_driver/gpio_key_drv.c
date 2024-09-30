#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>   // For copy_to_user
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/timer.h>      // For timer_setup() and container_of()

#define LONG_PRESS_THRESHOLD 500  // 500ms threshold for long press

struct gpio_key {
    int gpio;
    struct gpio_desc *gpiod;
    int flag;
    int irq;
    struct timer_list debounce_timer;  // Timer for debouncing
    unsigned long press_time;          // Time when the button is pressed
    int key_pressed;                   // Holds the key state
};

static struct gpio_key *gpio_keys_100ask;
static int major = 0;
static struct class *gpio_key_class;
static int g_key = 0;
static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);

/* Implement open/read/write functions, fill in file_operations structure */
static ssize_t gpio_key_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset) {
    int err;

    wait_event_interruptible(gpio_key_wait, g_key);  // Sleep when g_key = 0
    err = copy_to_user(buf, &g_key, 4);
    g_key = 0;

    return 4;
}

static struct file_operations gpio_key_drv = {
    .owner = THIS_MODULE,
    .read  = gpio_key_drv_read,
};

/* Handle quick press and long press */
static void classify_button_press(struct gpio_key *gpio_key, int val) {
    unsigned long duration;

    if (val == 1) {  // Button released
        duration = jiffies_to_msecs(jiffies - gpio_key->press_time);
        if (duration >= LONG_PRESS_THRESHOLD) {
            //printk("Long press detected on GPIO %d\n", gpio_key->gpio);
            g_key = (gpio_key->gpio << 8) | 2;  // Use 2 to indicate long press
        } else {
            //printk("Quick press detected on GPIO %d\n", gpio_key->gpio);
            g_key = (gpio_key->gpio << 8) | 1;  // Use 1 to indicate quick press
        }
        wake_up_interruptible(&gpio_key_wait);
    }
}

/* Debounce timer expiry function */
static void debounce_timer_expire(unsigned long data) {
    struct gpio_key *gpio_key = (struct gpio_key *)data;
    int val;

    val = gpiod_get_value(gpio_key->gpiod);  // Read final GPIO value after debouncing

    //printk("Debounced key %d %d\n", gpio_key->gpio, val);
    
    if (val == 0) {
        gpio_key->press_time = jiffies;  // Record the time when the button is pressed
    } else {
        classify_button_press(gpio_key, val);  // Classify quick press or long press
    }
}

/* Interrupt handler */
static irqreturn_t gpio_key_isr(int irq, void *dev_id) {
    struct gpio_key *gpio_key = dev_id;

    // Set debounce timer for 20ms
    mod_timer(&gpio_key->debounce_timer, jiffies + msecs_to_jiffies(20));

    return IRQ_HANDLED;
}

/* Probe function to initialize GPIOs and interrupts */
static int gpio_key_probe(struct platform_device *pdev) {
    int err;
    struct device_node *node = pdev->dev.of_node;
    int count;
    int i;
    enum of_gpio_flags flag;

    printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    count = of_gpio_count(node);
    if (!count) {
        printk("%s %s line %d, there isn't any gpio available\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    gpio_keys_100ask = kzalloc(sizeof(struct gpio_key) * count, GFP_KERNEL);
    for (i = 0; i < count; i++) {
        gpio_keys_100ask[i].gpio = of_get_gpio_flags(node, i, &flag);
        if (gpio_keys_100ask[i].gpio < 0) {
            printk("%s %s line %d, of_get_gpio_flags failed\n", __FILE__, __FUNCTION__, __LINE__);
            return -1;
        }

        gpio_keys_100ask[i].gpiod = gpio_to_desc(gpio_keys_100ask[i].gpio);
        gpio_keys_100ask[i].flag = flag & OF_GPIO_ACTIVE_LOW;
        gpio_keys_100ask[i].irq  = gpio_to_irq(gpio_keys_100ask[i].gpio);

        // Initialize the debounce timer
		setup_timer(&gpio_keys_100ask[i].debounce_timer, debounce_timer_expire, (unsigned long)&gpio_keys_100ask[i]);
    }

    for (i = 0; i < count; i++) {
        err = request_irq(gpio_keys_100ask[i].irq, gpio_key_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "100ask_gpio_key", &gpio_keys_100ask[i]);
        if (err) {
            printk("Failed to request IRQ for GPIO %d\n", gpio_keys_100ask[i].gpio);
        }
    }

    // Register device
    major = register_chrdev(0, "100ask_gpio_key", &gpio_key_drv);  /* /dev/100ask_gpio_key */
    gpio_key_class = class_create(THIS_MODULE, "100ask_gpio_key_class");
    if (IS_ERR(gpio_key_class)) {
        unregister_chrdev(major, "100ask_gpio_key");
        return PTR_ERR(gpio_key_class);
    }

    device_create(gpio_key_class, NULL, MKDEV(major, 0), NULL, "100ask_gpio_key"); /* /dev/100ask_gpio_key */

    return 0;
}

/* Remove function */
static int gpio_key_remove(struct platform_device *pdev) {
    struct device_node *node = pdev->dev.of_node;
    int count;
    int i;

    device_destroy(gpio_key_class, MKDEV(major, 0));
    class_destroy(gpio_key_class);
    unregister_chrdev(major, "100ask_gpio_key");

    count = of_gpio_count(node);
    for (i = 0; i < count; i++) {
        free_irq(gpio_keys_100ask[i].irq, &gpio_keys_100ask[i]);
        del_timer_sync(&gpio_keys_100ask[i].debounce_timer);  // Delete the debounce timer
    }
    kfree(gpio_keys_100ask);
    return 0;
}

static const struct of_device_id ask100_keys[] = {
    { .compatible = "100ask,gpio_key" },
    { },
};

static struct platform_driver gpio_keys_driver = {
    .probe      = gpio_key_probe,
    .remove     = gpio_key_remove,
    .driver     = {
        .name   = "100ask_gpio_key",
        .of_match_table = ask100_keys,
    },
};

static int __init gpio_key_init(void) {
    return platform_driver_register(&gpio_keys_driver);
}

static void __exit gpio_key_exit(void) {
    platform_driver_unregister(&gpio_keys_driver);
}

module_init(gpio_key_init);
module_exit(gpio_key_exit);

MODULE_LICENSE("GPL");




