#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define NO_OF_DEVICES 4

#undef pr_fmt
#define pr_fmt(fmt) "%s:" fmt, __func__

#define DEV_MEM_SIZE_MAX_PCDEV1 1024
#define DEV_MEM_SIZE_MAX_PCDEV2 512
#define DEV_MEM_SIZE_MAX_PCDEV3 1024
#define DEV_MEM_SIZE_MAX_PCDEV4 512

/* pseudo device memory */
char device_buffer_pcdev1[DEV_MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[DEV_MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[DEV_MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[DEV_MEM_SIZE_MAX_PCDEV4];


/* Device private data structure */
struct pcdev_private_data
{
  char *buffer;
  unsigned size;
  const char *serial_number;
  int perm;
  struct cdev cdev;
};

/* Driver private data structure */
struct pcdrv_private_data
{
  int total_devices;
  /* this holds device number */
  dev_t device_number;
  struct class *class_pcd;
  struct device *device_pcd;
  struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};

struct pcdrv_private_data pcdrv_data = 
{
  .total_devices = NO_OF_DEVICES,
  .pcdev_data = {
    [0] = {
      .buffer = device_buffer_pcdev1,
      .size = DEV_MEM_SIZE_MAX_PCDEV1,
      .serial_number = "PCDV1XYZ123",
      .perm = 0x1 /* RDONLY */
    },
    [1] = {
      .buffer = device_buffer_pcdev2,
      .size = DEV_MEM_SIZE_MAX_PCDEV2,
      .serial_number = "PCDV2XYZ123",
      .perm = 0x10 /* WRONLY */
    },
    [2] = {
      .buffer = device_buffer_pcdev3,
      .size = DEV_MEM_SIZE_MAX_PCDEV3,
      .serial_number = "PCDV3XYZ123",
      .perm = 0x11 /* RDWR */
    },
    [3] = {
      .buffer = device_buffer_pcdev4,
      .size = DEV_MEM_SIZE_MAX_PCDEV4,
      .serial_number = "PCDV4XYZ123",
      .perm = 0x11 /* RDWR */
    }
  }
};


loff_t pcd_lseek (struct file *filep, loff_t offset, int whence)
{
#if 0
  loff_t temp;

  pr_info("lseek requested \n");
  pr_info("Current file posiiton = %lld\n", filep->f_pos);

  switch(whence)
  {
    case SEEK_SET:
      if((offset > DEV_MEM_SIZE) || (offset < 0))
      {
        return -EINVAL;
      }
      filep->f_pos = offset;
      break;
    case SEEK_CUR:
      temp = filep->f_pos + offset;
      if((temp > DEV_MEM_SIZE) || (temp < 0))
      {
        return -EINVAL;
      }
      filep->f_pos = temp;;
      break;
    case SEEK_END:
      temp = DEV_MEM_SIZE + offset;
      if((temp > DEV_MEM_SIZE) || (temp < 0))
      {
        return -EINVAL;
      }
      filep->f_pos = temp;
      break;
    default:
      return -EINVAL;;
  }
  
  pr_info("New value of the file position = %lld\n", filep->f_pos);

  return filep->f_pos;
#endif
  return 0;
}

ssize_t pcd_read (struct file *filep, char __user *buff, size_t count, loff_t *f_pos)
{
#if 0
  pr_info("Read requsted for %zu bytes\n", count);
  pr_info("Current file position = %lld\n", *f_pos);

  /* Adjust the 'count' */
  if(*f_pos + count > DEV_MEM_SIZE)
  {
    count = DEV_MEM_SIZE - *f_pos;
  }
 

  /* Copy to user */
  if(copy_to_user(buff, &device_buffer[*f_pos], count))
  {
    return -EFAULT;
  }


  /* Update the current file position */
  *f_pos += count;

  pr_info("Number of bytes succesfully read = %zu bytes\n", count);
  pr_info("Updated file position = %lld\n", *f_pos);

  /* Return number of bytes which have been succesfully copied */
  return count;
#endif
  return 0;
}

ssize_t pcd_write (struct file *filep, const char __user *buff, size_t count, loff_t *f_pos)
{
#if 0
  pr_info("write requested for %zu bytes \n", count);
  pr_info("Current file position = %lld\n", *f_pos);

  /* Adjust the 'count' */
  if(*f_pos + count > DEV_MEM_SIZE)
  {
    count = DEV_MEM_SIZE - *f_pos;
  }
 

  /* Copy from user */
  if(copy_from_user(&device_buffer[*f_pos], buff, count))
  {
    return -EFAULT;
  }

  if(!count)
  {
    pr_err("No space left on the device.\n");
    return -ENOMEM;
  }


  /* Update the current file position */
  *f_pos += count;

  pr_info("Number of bytes succesfully written = %zu bytes\n", count);
  pr_info("Updated file position = %lld\n", *f_pos);

  /* Return number of bytes which have been succesfully written */
  return count;
#endif
  return -ENOMEM;
}

int pcd_open (struct inode *inode, struct file *filep)

{
  pr_info("Open was succesfull \n");


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


static int __init pcd_driver_init(void)
{
#if 0
  int ret;

  /*1. Dynamicaly allocate a device number */
  ret = alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");
  if(ret < 0)
  {
    pr_err("Alloc chrdev failed.\n");
    goto out;
  }

  pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

  /*2. Initialize the cdev structure with fops */
  cdev_init(&pcd_cdev, &pcd_fops);

  /*3. Register c device (dev structure) with VFS */
  pcd_cdev.owner = THIS_MODULE;
  ret =  cdev_add(&pcd_cdev, device_number, 1);
  if(ret < 0)
    goto unreg_chardev;

  /*4.Clearte device class under /sys/class/ */
  class_pcd = class_create(THIS_MODULE, "pcd_class");
  if(IS_ERR(class_pcd))
  {
    pr_info("Class cration failed.\n");
    ret = PTR_ERR(class_pcd);
    goto cdev_delete;
  }

  /*5. Populate sysfs with device information*/
  device_pcd =  device_create(class_pcd, NULL, device_number, NULL, "pcd");
  if(IS_ERR(device_pcd))
  {
    pr_info("Device creationfailed.\n");
    ret = PTR_ERR(device_pcd);
    goto class_destr;
  }

  pr_info("Module init was successful\n");

  return 0;

class_destr:
  device_destroy(class_pcd, device_number);

cdev_delete:
  cdev_del(&pcd_cdev);

unreg_chardev:
  unregister_chrdev_region(device_number, 1);

out:
  return ret;
#endif
  return 0;
}

static void  __exit pcd_driver_cleanup(void)
{
#if 0
  device_destroy(class_pcd, device_number);
  class_destroy(class_pcd);
  cdev_del(&pcd_cdev);
  unregister_chrdev_region(device_number, 1);

  pr_info("Module unloaded\n");
#endif
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kamil Piekutowski");
MODULE_DESCRIPTION("A pseudo character driver for n-devices");

