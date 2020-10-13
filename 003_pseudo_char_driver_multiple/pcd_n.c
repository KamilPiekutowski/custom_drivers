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

  struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*) filep->private_data;

  int max_size = pcdev_data->size;
  loff_t temp;

  pr_info("lseek requested \n");
  pr_info("Current file posiiton = %lld\n", filep->f_pos);

  switch(whence)
  {
    case SEEK_SET:
      if((offset > max_size) || (offset < 0))
      {
        return -EINVAL;
      }
      filep->f_pos = offset;
      break;
    case SEEK_CUR:
      temp = filep->f_pos + offset;
      if((temp > max_size) || (temp < 0))
      {
        return -EINVAL;
      }
      filep->f_pos = temp;;
      break;
    case SEEK_END:
      temp = max_size + offset;
      if((temp > max_size) || (temp < 0))
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
}

ssize_t pcd_read (struct file *filep, char __user *buff, size_t count, loff_t *f_pos)
{
  struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*) filep->private_data;

  int max_size = pcdev_data->size;


  pr_info("Read requsted for %zu bytes\n", count);
  pr_info("Current file position = %lld\n", *f_pos);

  /* Adjust the 'count' */
  if(*f_pos + count > max_size)
  {
    count = max_size - *f_pos;
  }


  /* Copy to user */
  if(copy_to_user(buff, &pcdev_data->buffer + (*f_pos), count))
  {
    return -EFAULT;
  }


  /* Update the current file position */
  *f_pos += count;

  pr_info("Number of bytes succesfully read = %zu bytes\n", count);
  pr_info("Updated file position = %lld\n", *f_pos);

  /* Return number of bytes which have been succesfully copied */
  return count;
}

ssize_t pcd_write (struct file *filep, const char __user *buff, size_t count, loff_t *f_pos)
{
  struct pcdev_private_data *pcdev_data = (struct pcdev_private_data*) filep->private_data;

  int max_size = pcdev_data->size;

  pr_info("write requested for %zu bytes \n", count);
  pr_info("Current file position = %lld\n", *f_pos);

  /* Adjust the 'count' */
  if(*f_pos + count > max_size)
  {
    count = max_size - *f_pos;
  }


  /* Copy from user */
  if(copy_from_user(&pcdev_data->buffer + (*f_pos), buff, count))
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
}

int check_permission(void)
{
  return 0;
}

int pcd_open (struct inode *inode, struct file *filep)
{
  int ret;
  int minor_num;

  struct pcdev_private_data *pcdev_data;

  /* Find out on which device file open was attempted by the user space  */

  minor_num = MINOR(inode->i_rdev);
  pr_info("Minor number access = %d\n", minor_num);

  /* Get device's private date structure */
  pcdev_data = container_of(inode->i_cdev, struct pcdev_private_data, cdev);

  filep->private_data = pcdev_data;

  /* Check permission */
  ret = check_permission();

  (!ret) ? pr_info("Open was successful.\n") : pr_info("Open failed.\n");

  return ret;
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
  int ret;
  int i;

  /* Dynamicaly allocate a device numbers */
  ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcd_devices");
  if(ret < 0)
  {
    pr_err("Alloc chrdev failed.\n");
    goto out;
  }

  /* Create device class under /sys/class/ */
  pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
  if(IS_ERR(pcdrv_data.class_pcd))
  {
    pr_info("Class cration failed.\n");
    ret = PTR_ERR(pcdrv_data.class_pcd);
    goto unreg_chardev;
  }

  for(i =0; i < NO_OF_DEVICES; ++i)
  {
    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(pcdrv_data.device_number+i), MINOR(pcdrv_data.device_number+i));

    /* Initialize the cdev structure with fops */
    cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);

    /* Register c device (dev structure) with VFS */
    pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;
    ret =  cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number+i, 1);
    if(ret < 0)
      goto cdev_del;


    /* Populate sysfs with device information*/
    pcdrv_data.device_pcd =  device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number+i, NULL, "pcdev-%d", i);
    if(IS_ERR(pcdrv_data.device_pcd))
    {
      pr_info("Device creationfailed.\n");
      ret = PTR_ERR(pcdrv_data.device_pcd);
      goto class_del;
    }
  }

  pr_info("Module init was successful\n");

  return 0;

cdev_del:
class_del:
  for(; i >= 0; i--)
  {
    device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
    cdev_del(&pcdrv_data.pcdev_data[i].cdev);
  }

  class_destroy(pcdrv_data.class_pcd);


unreg_chardev:
  unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

out:
  return ret;
}

static void  __exit pcd_driver_cleanup(void)
{
  int i;
  for(i = 0; i < NO_OF_DEVICES; i++)
  {
    device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
    cdev_del(&pcdrv_data.pcdev_data[i].cdev);
  }

  class_destroy(pcdrv_data.class_pcd);

  unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

  pr_info("Module unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kamil Piekutowski");
MODULE_DESCRIPTION("A pseudo character driver for n-devices");

