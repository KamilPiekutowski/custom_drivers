#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s:" fmt, __func__

#define DEV_MEM_SIZE 512

/* pseudo device memory */
char device_buffer[DEV_MEM_SIZE];

/* this holds device number */
dev_t device_number;

/*  Cdev variable */
struct cdev pcd_cdev;

loff_t pcd_lseek (struct file *filep, loff_t off, int whence)
{
  pr_info("lseek requested \n");
  return 0;
}

ssize_t pcd_read (struct file *filep, char __user *buff, size_t count, loff_t *f_pos)
{
  pr_info("read requsted for %zu bytes \n", count);
  return 0;
}

ssize_t pcd_write (struct file *filep, const char __user *buff, size_t count, loff_t *f_pos)
{
  pr_info("write requested for %zu bytes \n", count);
  return 0;
}

int pcd_open (struct inode *inode, struct file *filep)
{
  pr_info("open was succesfull \n");
  return 0;
}

int pcd_release (struct inode *inode, struct file *filep)
{
  pr_info("close was succesful \n");
  return 0;
}

/* file operation of the driver */
struct file_operations pcd_fops = 
{
  .llseek  = pcd_lseek,
  .read    = pcd_read,
  .write   = pcd_write,
  .open    = pcd_open,
  .release = pcd_release,
  .owner   = THIS_MODULE,
};

struct class *class_pcd;

struct device *device_pcd;


static int __init pcd_driver_init(void)
{
  /*1. Dynamicaly allocate a device number */
  alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");

  pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

  /*2. Initialize the cdev structure with fops */
  cdev_init(&pcd_cdev, &pcd_fops);

  /*3. Register c device (dev structure) with VFS */
  pcd_cdev.owner = THIS_MODULE;
  cdev_add(&pcd_cdev, device_number, 1);


  /*4.Clearte device class under /sys/class/ */
  class_pcd = class_create(THIS_MODULE, "pcd_class");

  /*5. Populate sysfs with device information*/
  device_pcd =  device_create(class_pcd, NULL, device_number, NULL, "pcd");

  pr_info("Module init was successful\n");

  return 0;
}

static void  __exit pcd_driver_cleanup(void)
{
  device_destroy(class_pcd, device_number);
  class_destroy(class_pcd);
  cdev_del(&pcd_cdev);
  unregister_chrdev_region(device_number, 1);

  pr_info("Module unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kamil Piekutowski");
MODULE_DESCRIPTION("A pseudo character driver");
