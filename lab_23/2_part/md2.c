#include <linux/init.h>
#include <linux/module.h>
#include "md.h"


MODULE_LICENSE("GPL");


static int __init md_init(void) 
{
    printk("+ Module md2 started!\n"); 
    printk("+ Data string exported from md1 : %s\n", md1_data); 
    printk("+ String returned md1_proc() is : %s\n", md1_proc()); 


    printk("+ String returned md1_noexport() is : %s\n", md1_local()); 
    
    return 0; 
} 

static void __exit md_exit(void) 
{
    printk("+ Module md2 unloaded!\n"); 
} 

module_init(md_init); 
module_exit(md_exit);