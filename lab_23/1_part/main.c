#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");  


static int __init md_init(void)
{
    printk("Module md loaded\n");

    struct task_struct *task = &init_task;

    do{
        printk(KERN_INFO "Name: %s id: %d\tParent Name: %s Parent id:%d\n", 
                task->comm, task->pid,
                task->parent->comm, task->parent->pid);
    }while ((task = next_task(task)) != init_task);

    printk(KERN_INFO "Cur process: name = %s\tid = %d\n", 
                current->comm, current->pid);

    return 0;
}

static void __exit md_exit(void)
{
    printk("Module md exited");
}


module_init(md_init);
module_exit(md_exit);
