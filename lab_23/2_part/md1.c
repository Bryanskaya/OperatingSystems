#include <linux/module.h>
#include "md.h"


char *md1_data = "Hello, world!";

extern char *md1_proc(void)
{
    return md1_data;
}

static char *md1_local(void)
{
    return md1_data;
}

extern char *md1_noexport(void)
{
    return md1_data;
}

EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_proc);

static int __init md_init(void)
{
    printk("+ Module md1 start\n);
    return 0;
}

static int __exit md_exit(void)
{
    printk("+ Module md1 unloaded\n);
}

module_init(md_init);
module_exit(md_exit);