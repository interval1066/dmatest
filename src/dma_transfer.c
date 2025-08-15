#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/module.h>

/* File operations structures */
static int dma_in_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "DMA: Input device opened\n");
    return 0;
}

static ssize_t dma_in_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    printk(KERN_INFO "DMA: Read %zu bytes from input\n", count);
    return 0;
}

static const struct file_operations fops_in = {
    .owner = THIS_MODULE,
    .open = dma_in_open,
    .read = dma_in_read,
};

static int dma_out_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "DMA: Output device opened\n");
    return 0;
}

static ssize_t dma_out_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    printk(KERN_INFO "DMA: Wrote %zu bytes to output\n", count);
    return count;
}

static const struct file_operations fops_out = {
    .owner = THIS_MODULE,
    .open = dma_out_open,
    .write = dma_out_write,
};

/* Global variables */
static struct class *dma_class;
static struct device *dev_in, *dev_out;
static struct cdev dma_cdev_in, dma_cdev_out;
static int major_in, major_out;

static int __init mod_init(void)
{
    dev_t dev_num_in, dev_num_out;
    int ret;

    printk(KERN_INFO "DMA: Initializing dual-channel module\n");

    /* --- Input Device Setup --- */
    ret = alloc_chrdev_region(&dev_num_in, 0, 1, "dma_in");
    if (ret < 0) {
        printk(KERN_ERR "DMA: Failed to allocate input device numbers\n");
        return ret;
    }
    major_in = MAJOR(dev_num_in);

    /* Correct class_create() for modern kernels */
    dma_class = class_create("dma_devices");
    if (IS_ERR(dma_class)) {
        ret = PTR_ERR(dma_class);
        printk(KERN_ERR "DMA: Failed to create device class (%d)\n", ret);
        goto err_unreg_in;
    }

    dev_in = device_create(dma_class, NULL, dev_num_in, NULL, "dma_in");
    if (IS_ERR(dev_in)) {
        ret = PTR_ERR(dev_in);
        printk(KERN_ERR "DMA: Failed to create input device (%d)\n", ret);
        goto err_class;
    }

    cdev_init(&dma_cdev_in, &fops_in);
    ret = cdev_add(&dma_cdev_in, dev_num_in, 1);
    if (ret < 0) {
        printk(KERN_ERR "DMA: Failed to add input cdev (%d)\n", ret);
        goto err_dev_in;
    }

    /* --- Output Device Setup --- */
    ret = alloc_chrdev_region(&dev_num_out, 0, 1, "dma_out");
    if (ret < 0) {
        printk(KERN_ERR "DMA: Failed to allocate output device numbers (%d)\n", ret);
        goto err_cdev_in;
    }
    major_out = MAJOR(dev_num_out);

    dev_out = device_create(dma_class, NULL, dev_num_out, NULL, "dma_out");
    if (IS_ERR(dev_out)) {
        ret = PTR_ERR(dev_out);
        printk(KERN_ERR "DMA: Failed to create output device (%d)\n", ret);
        goto err_unreg_out;
    }

    cdev_init(&dma_cdev_out, &fops_out);
    ret = cdev_add(&dma_cdev_out, dev_num_out, 1);
    if (ret < 0) {
        printk(KERN_ERR "DMA: Failed to add output cdev (%d)\n", ret);
        goto err_dev_out;
    }

    printk(KERN_INFO "DMA: Successfully created:\n");
    printk(KERN_INFO "     /dev/dma_in (major %d, minor 0)\n", major_in);
    printk(KERN_INFO "     /dev/dma_out (major %d, minor 0)\n", major_out);
    return 0;

/* Error handling */
err_dev_out:
    device_destroy(dma_class, dev_num_out);
err_unreg_out:
    unregister_chrdev_region(dev_num_out, 1);
err_cdev_in:
    cdev_del(&dma_cdev_in);
err_dev_in:
    device_destroy(dma_class, dev_num_in);
err_class:
    class_destroy(dma_class);
err_unreg_in:
    unregister_chrdev_region(dev_num_in, 1);
    return ret;
}

static void __exit mod_exit(void)
{
    dev_t dev_num_in = MKDEV(major_in, 0);
    dev_t dev_num_out = MKDEV(major_out, 0);

    /* Clean up output device */
    cdev_del(&dma_cdev_out);
    device_destroy(dma_class, dev_num_out);
    unregister_chrdev_region(dev_num_out, 1);

    /* Clean up input device */
    cdev_del(&dma_cdev_in);
    device_destroy(dma_class, dev_num_in);
    unregister_chrdev_region(dev_num_in, 1);

    /* Clean shared resources */
    class_destroy(dma_class);

    printk(KERN_INFO "DMA: Module unloaded\n");
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Dual-channel DMA device driver");

