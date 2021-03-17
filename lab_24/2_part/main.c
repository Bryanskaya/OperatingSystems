#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>

#include <init.h>
#include <linux/string.h>


MODULE_LICENSE("GPL");


static struct proc_dir_entry *proc_entry;


char* cookie_pot;

static int init_fortune_module()
{
    cookie_pot = (char*)vmalloc(MAX_COOKIE_LENGTH);
    if (!cookie_pot)
    {
        printk(KERN_INFO "fortune: impossible to malloc cookie_pot");
        return -ENOMEM;
    }

    memset(cookie_pot, 0, MAX_COOKIE_LENGTH);
    proc_entry = proc_create(?????????);

    if (!proc_entry)
    {
        vfree(cookie_pot);
        printk(KERN_INFO "fortune: can't create proc entry");
        return -ENOMEM;
    }

    printk(KERN_INFO "===== Fortune module loaded! =====");

    return 0;
}

static void exit_fortune_module()
{
    printk(KERN_INFO "===== Fortune module unloaded! =====");
}



module_init(init_fortune_module);
module_exit(exit_fortune_module);