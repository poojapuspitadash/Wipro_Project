// Smart Energy Meter Linux character driver
// Software-only prototype: user space can inject pulses through write().

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include "smartmeter_ioctl.h"

#define DEVICE_NAME "smartmeter"
#define CLASS_NAME "smartmeter"

static dev_t smartmeter_dev;
static struct cdev smartmeter_cdev;
static struct class *smartmeter_class;
static struct device *smartmeter_device;

static unsigned long pulse_count;
static DEFINE_MUTEX(pulse_lock);

static int smartmeter_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int smartmeter_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t smartmeter_read(struct file *file, char __user *buffer,
                               size_t len, loff_t *offset)
{
    char text[32];
    int n;
    unsigned long count;

    if (*offset != 0)
        return 0;

    mutex_lock(&pulse_lock);
    count = pulse_count;
    mutex_unlock(&pulse_lock);

    n = scnprintf(text, sizeof(text), "%lu\n", count);
    if (len < n)
        return -EINVAL;

    if (copy_to_user(buffer, text, n))
        return -EFAULT;

    *offset += n;
    return n;
}

static ssize_t smartmeter_write(struct file *file, const char __user *buffer,
                                size_t len, loff_t *offset)
{
    char text[32];
    unsigned long pulses;

    if (len == 0 || len >= sizeof(text))
        return -EINVAL;

    if (copy_from_user(text, buffer, len))
        return -EFAULT;

    text[len] = '\0';

    if (kstrtoul(text, 10, &pulses))
        return -EINVAL;

    mutex_lock(&pulse_lock);
    pulse_count += pulses;
    mutex_unlock(&pulse_lock);

    return len;
}

static long smartmeter_ioctl(struct file *file, unsigned int cmd,
                             unsigned long arg)
{
    unsigned long value;

    switch (cmd) {
    case SMARTMETER_RESET:
        mutex_lock(&pulse_lock);
        pulse_count = 0;
        mutex_unlock(&pulse_lock);
        return 0;

    case SMARTMETER_GET_COUNT:
        mutex_lock(&pulse_lock);
        value = pulse_count;
        mutex_unlock(&pulse_lock);
        if (copy_to_user((unsigned long __user *)arg, &value, sizeof(value)))
            return -EFAULT;
        return 0;

    case SMARTMETER_SET_COUNT:
        if (copy_from_user(&value, (unsigned long __user *)arg, sizeof(value)))
            return -EFAULT;
        mutex_lock(&pulse_lock);
        pulse_count = value;
        mutex_unlock(&pulse_lock);
        return 0;

    default:
        return -ENOTTY;
    }
}

static const struct file_operations smartmeter_fops = {
    .owner = THIS_MODULE,
    .open = smartmeter_open,
    .release = smartmeter_release,
    .read = smartmeter_read,
    .write = smartmeter_write,
    .unlocked_ioctl = smartmeter_ioctl,
};

static int __init smartmeter_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&smartmeter_dev, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&smartmeter_cdev, &smartmeter_fops);
    smartmeter_cdev.owner = THIS_MODULE;

    ret = cdev_add(&smartmeter_cdev, smartmeter_dev, 1);
    if (ret < 0)
        goto unregister_region;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    smartmeter_class = class_create(CLASS_NAME);
#else
    smartmeter_class = class_create(THIS_MODULE, CLASS_NAME);
#endif
    if (IS_ERR(smartmeter_class)) {
        ret = PTR_ERR(smartmeter_class);
        goto del_cdev;
    }

    smartmeter_device = device_create(smartmeter_class, NULL, smartmeter_dev,
                                      NULL, DEVICE_NAME);
    if (IS_ERR(smartmeter_device)) {
        ret = PTR_ERR(smartmeter_device);
        goto destroy_class;
    }

    mutex_lock(&pulse_lock);
    pulse_count = 0;
    mutex_unlock(&pulse_lock);

    pr_info("smartmeter: loaded major=%d minor=%d\n",
            MAJOR(smartmeter_dev), MINOR(smartmeter_dev));
    return 0;

destroy_class:
    class_destroy(smartmeter_class);
del_cdev:
    cdev_del(&smartmeter_cdev);
unregister_region:
    unregister_chrdev_region(smartmeter_dev, 1);
    return ret;
}

static void __exit smartmeter_exit(void)
{
    device_destroy(smartmeter_class, smartmeter_dev);
    class_destroy(smartmeter_class);
    cdev_del(&smartmeter_cdev);
    unregister_chrdev_region(smartmeter_dev, 1);
    pr_info("smartmeter: unloaded\n");
}

module_init(smartmeter_init);
module_exit(smartmeter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Wipro Training Project");
MODULE_DESCRIPTION("Smart Energy Meter pulse counter character device driver");
MODULE_VERSION("1.0");
