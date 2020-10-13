#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>

#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s:" fmt, __func__

int check_permission(int dev_perm, int access_mode)
{
  if(dev_perm == RDWR)
    return 0;
  if((dev_perm == RDONLY) && ((access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE) )) 
    return 0;

  if((dev_perm == WRONLY) && ((access_mode & FMODE_WRITE) && !(access_mode & FMODE_READ) ))
    return 0;

  return -EPERM;
}

loff_t pcd_lseek (struct file *filep, loff_t offset, int whence)
{
  return 0;
}


ssize_t pcd_read (struct file *filep, char __user *buff, size_t count, loff_t *f_pos)
{
  return 0;
}

ssize_t pcd_write (struct file *filep, const char __user *buff, size_t count, loff_t *f_pos)
{
  return -ENOMEM;
}

int pcd_open (struct inode *inode, struct file *filep)
{
  return 0;
}

int pcd_release (struct inode *inode, struct file *filep)
{
  pr_info("Release was succesful \n");
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

/* This function gets called when the device is removed from the system */
int pcd_platform_driver_remove(struct platform_device *pdev)
{
  return 0;
}

/* This function gets called when matched platform is found */
int pcd_platform_driver_probe(struct platform_device *pdev)
{
  return 0;
}


struct platform_driver pcd_platform_driver = 
{
  .probe = pcd_platform_driver_probe,
  .remove = pcd_platform_driver_remove,
  .driver = {
    .name = "pseudo-char-device"
  }
};

static int __init pcd_platform_driver_init(void)
{
  platform_driver_register(&pcd_platform_driver);
  pr_info("pcd platform driver loaded\n");

  return 0;
}


static void  __exit pcd_platform_driver_cleanup(void)
{
  platform_driver_unregister(&pcd_platform_driver);
  pr_info("pcd platform driver unloaded\n");

}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kamil Piekutowski");
MODULE_DESCRIPTION("A pseudo character platform driver to handle n platform pcdes");
