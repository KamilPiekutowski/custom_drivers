#include <linux/module.h>

static int __init helloworld_init(void)
{
  pr_info("Hello world\n");
  return 0;
}

static void __exit helloworld_cleanup(void)
{
  pr_info("Goddbye world!\n");
}

module_init(helloworld_init);
module_exit(helloworld_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kamil Piekutowski");
MODULE_DESCRIPTION("A simple hello world");
MODULE_INFO(board, "Beaglebone Black REv C");
