#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/sysfs.h>
#include<sys/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nt");
MODULE_DESCRIPTION("");

dev_t dev = 0;
struct kobject *kobj_ref;
volatile int sysfs_file = 0;
static struct class *dev_class;
static struct cdev cdev_dri;

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

// driver function prototypes
static int      dev_file_open(struct inode *inode, struct file *file);
static int      dev_file_release(struct inode *inode, struct file *file);
static ssize_t  dev_file_read(struct file *filp,
                        char __user *buf, size_t len,loff_t * off);
static ssize_t  dev_file_write(struct file *filp,
                        const char *buf, size_t len, loff_t * off);

// sysfs function prototypes
static ssize_t  sysfs_show(struct kobject *kobj,
                        struct kobj_attribute *attr, char *buf);
static ssize_t  sysfs_store(struct kobject *kobj,
                        struct kobj_attribute *attr,const char *buf, size_t count);

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

struct kobj_attribute dri_attr = __ATTR(sysfs_file, 0660, sysfs_show, sysfs_store);

// File operation sturcture
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = dev_file_read,
        .write          = dev_file_write,
        .open           = dev_file_open,
        .release        = dev_file_release,
        .unlocked_ioctl = dev_ioctl,
};

// called when sysfs file is read
static ssize_t sysfs_show(struct kobject *kobj,
                struct kobj_attribute *attr, char *buf)
{
        printk("read from sysfs\n");
        return sprintf(buf, "%d", sysfs_file);
}

// called when sysfs file is written
static ssize_t sysfs_store(struct kobject *kobj,
                struct kobj_attribute *attr,const char *buf, size_t count)
{
        printk("written to sysfs\n");
        sscanf(buf,"%d",&sysfs_file);
        return count;
}

// called when device file is opened
static int dev_file_open(struct inode *inode, struct file *file)
{
        printk("dev file opened\n");
        return 0;
}

// called device file is closed
static int dev_file_release(struct inode *inode, struct file *file)
{
        printk("dev file closed\n");
        return 0;
}

// called when device file is read
static ssize_t dev_file_read(struct file *filp,
                char __user *buf, size_t len, loff_t *off)
{
        printk("read from dev file\n");
        return 0;
}

// called device file is written into
static ssize_t dev_file_write(struct file *filp,
                const char __user *buf, size_t len, loff_t *off)
{
        printk("written to dev file\n");
        return len;
}

// ioctl function
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch(cmd) {
    case WR_VALUE:
        printk("IOCTL write for %d\n", (int)arg);
        break;
    case RD_VALUE:
        printk("IOCTL read for %d\n", (int)arg);
        break;
    default:
        printk("invalid IOCTL command\n");
    }
    return 0;
}


// driver init function
static int cdev_dri_init(void)
{
    printk(KERN_INFO "Initializing Module.\n");

    // allocate major number
    if((alloc_chrdev_region(&dev, 0, 1, "cdev_dri")) <0){
            printk("Failed to allocate major number.\n");
            return -1;
    }
    printk("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    // create cdev structure
    cdev_init(&cdev_dri, &fops);

    // add character device to the system
    if((cdev_add(&cdev_dri, dev, 1)) < 0){
        printk("Failed to add the device to the system.\n");
        goto r_class;
    }

    // create struct class
    if((dev_class = class_create(THIS_MODULE, "cdev_dri_class")) == NULL){
        printk("Failed to create the struct class.\n");
        goto r_class;
    }

    // create device
    if((device_create(dev_class, NULL, dev, NULL, "cdev_dri_device")) == NULL){
        printk("Failed to create the device.\n");
        goto r_device;
    }

    // create a dir in /sys/kernel/
    kobj_ref = kobject_create_and_add("cdev_dri_app",kernel_kobj);

    // create sysfs file for sysfs_file
    if(sysfs_create_file(kobj_ref,&dri_attr.attr)){
            pr_err("Failed to create sysfs file.\n");
            goto r_sysfs;
    }
    printk("Driver init completed.\n");

    return 0;

r_sysfs:
    kobject_put(kobj_ref);
    sysfs_remove_file(kernel_kobj, &dri_attr.attr);

r_device:
        class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    cdev_del(&cdev_dri);

    return -1;
}

// driver cleanup/exit function
static void __exit cdev_dri_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");

    // free Kobj
    kobject_put(kobj_ref);
    sysfs_remove_file(kernel_kobj, &dri_attr.attr);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&cdev_dri);
    unregister_chrdev_region(dev, 1);
}

module_init(cdev_dri_init);
module_exit(cdev_dri_cleanup);
