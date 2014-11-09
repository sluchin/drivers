#include <asm/uaccess.h>    /* copy_from_user copy_to_user */
#include <linux/errno.h>
#include <linux/fs.h>       /* inode file file_operations */
#include <linux/kernel.h>   /* printk */
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/kdev_t.h>   /* MAJOR */
#include <linux/slab.h>     /* kmalloc */
#include <linux/cdev.h>     /* struct cdev */

#include "ring.h"
#include "log.h"

static char* devname = "chardev";
static dev_t devid;
static struct cdev cdev;
static spinlock_t lock;

static int chardev_open(struct inode* inode, struct file* filp);
static int chardev_release(struct inode* inode, struct file* filp);
static ssize_t chardev_read(struct file* filp, char* buf, size_t count, loff_t* offset);
static ssize_t chardev_write(struct file* filp, const char* buf, size_t count, loff_t* offset);
static int __init chardev_init_module(void);
static void __exit chardev_exit_module(void);

/**
 * open
 */
static int
chardev_open(struct inode* inode, struct file* filp)
{
    kdebug("start: %d\n", MINOR(inode->i_rdev));
    spin_lock(&lock);
    filp->private_data = &inode->i_rdev;
    spin_unlock(&lock);
    return 0;
}

/**
 * release
 */
static int
chardev_release(struct inode* inode, struct file* filp)
{
    kdebug("start\n");
    return 0;
}

/**
 * read
 */
static ssize_t
chardev_read(struct file* filp, char* buf, size_t count, loff_t* offset)
{
    unsigned char ch = 0x00;
    int retval = 0;
    ssize_t pos = 0;
    dev_t dev;
    int minor;

    kdebug("start\n");
    kdebug("count=%d\n", count);

    spin_lock(&lock);

    dev = *((int *)filp->private_data);
	minor = MINOR(dev);
    kdebug("minor=%d\n", minor);
    while (pos < count) {
        retval = get(minor, &ch);
        if (!retval)
            break;
        if (copy_to_user(buf + pos, &ch, sizeof(ch))) {
            kerror("copy_to_user failed\n");
            return -EFAULT;
        }
        pos += 1;
    }
    offset += pos;

    spin_unlock(&lock);

    return pos;
}

/**
 * write
 */
static ssize_t
chardev_write(struct file* filp, const char* buf, size_t count, loff_t* offset)
{
    unsigned char ch = 0x00;
    ssize_t pos = 0;
    dev_t dev;
    int minor;

    kdebug("start\n");
    kdebug("count=%d\n", count);

    spin_lock(&lock);

    dev = *((int *)filp->private_data);
	minor = MINOR(dev);
    kdebug("minor=%d\n", minor);
    while (pos < count) {
        if (copy_from_user(&ch, buf + pos, sizeof(ch))) {
            kerror("copy_from_user failed\n");
            return -EFAULT;
        }
        (void)put(minor, ch);
        pos += 1;
    }
    offset += pos;

    spin_unlock(&lock);

    return pos;
}

static struct file_operations chardev_fops =
{
    owner   : THIS_MODULE,
    read    : chardev_read,
    write   : chardev_write,
    open    : chardev_open,
    release : chardev_release,
};

static int __init
chardev_init_module(void)
{
    int retval = 0;
    int devno = 0;
    int major = 0;

    retval = alloc_chrdev_region(&devid, 0, MINOR_COUNT, devname);
    if (retval < 0) {
        kerror("alloc_chrdev_region failed\n");
        goto alloc_error;
    }
    major = MAJOR(devid);
    devno = MKDEV(major, 0);
    tty_console("%s: %d\n", devname, major);

    cdev_init(&cdev, &chardev_fops);
    cdev.owner = THIS_MODULE;
    retval = cdev_add(&cdev, devno, MINOR_COUNT);
    if (retval) {
        kerror("cdev_add failed\n");
        goto add_error;
    }

    spin_lock_init(&lock);

    kinfo("loaded into kernel\n");
    return 0;

add_error:
    cdev_del(&cdev);
alloc_error:
    unregister_chrdev_region(devid, MINOR_COUNT);

    return -EBUSY;
}

static void __exit
chardev_exit_module(void)
{
    cdev_del(&cdev);
    unregister_chrdev_region(devid, MINOR_COUNT);
    kinfo("removed from kernel\n");
}

module_init(chardev_init_module);
module_exit(chardev_exit_module);

MODULE_AUTHOR("Tetsuya Higashi");
MODULE_DESCRIPTION("Sample character device driver.");
MODULE_LICENSE("GPL");
