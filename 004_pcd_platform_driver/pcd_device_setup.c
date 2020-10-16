#include <linux/module.h>
#include <linux/platform_device.h>

#include "platform.h"

/* Macro that prepends function name to pr_info*/
#undef pr_fmt
#define pr_fmt(fmt) "%s:" fmt, __func__

void pcdev_release(struct device *dev)
{
  pr_info("Device released");
}

/* 1. create 2 platform data structs */
struct pcdev_platform_data pcdev_pdata[] =
{
  [0] = { .size = 512 , .perm = RDWR,   .serial_number = "PCDEVABC1111",},
  [1] = { .size = 1024, .perm = RDWR,   .serial_number = "PCDEVABC2222",},
  [2] = { .size = 128,  .perm = RDONLY, .serial_number = "PCDEVABC3333",},
  [3] = { .size = 32,   .perm = WRONLY, .serial_number = "PCDEVABC4444",},
};

/* 12. create 2 platform devices */

struct platform_device platform_pcdev_1 = 
{
  .name = "pcdev-A1X",
  .id = 0,
  .dev = {
    .platform_data = &pcdev_pdata[0],
    .release = pcdev_release,
  }
};

struct platform_device platform_pcdev_2 = 
{
  .name = "pcdev-B1X",
  .id = 1,
  .dev = {
    .platform_data = &pcdev_pdata[1],
    .release = pcdev_release,
  }
};

struct platform_device platform_pcdev_3 = 
{
  .name = "pcdev-C1X",
  .id = 12,
  .dev = {
    .platform_data = &pcdev_pdata[2],
    .release = pcdev_release,
  }
};

struct platform_device platform_pcdev_4 = 
{
  .name = "pcdev-D1X",
  .id = 2,
  .dev = {
    .platform_data = &pcdev_pdata[3],
    .release = pcdev_release,
  }
};

struct platform_device *platform_pcdevs[] =
{
  &platform_pcdev_1,
  &platform_pcdev_2,
  &platform_pcdev_3,
  &platform_pcdev_4,
};

static int __init pcdev_platform_init(void)
{
  /* register platform device */
  //platform_device_register(&platform_pcdev_1);
  //platform_device_register(&platform_pcdev_2);

  platform_add_devices(platform_pcdevs, ARRAY_SIZE(platform_pcdevs));

  pr_info("Device setup module loaded\n");

  return 0;
}

static void __exit pcdev_platform_exit(void)
{
  platform_device_unregister(&platform_pcdev_1);
  platform_device_unregister(&platform_pcdev_2);
  platform_device_unregister(&platform_pcdev_3);
  platform_device_unregister(&platform_pcdev_4);

  pr_info("Device setup module unloaded\n");

}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module registering platform devices");
