#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");


static int __init md_init(void)
{
    printk("******************************* Module md loaded ************************\n");

    struct task_struct *task = &init_task;

    do{
        printk(KERN_INFO ">>>Name: %s \t id: %d \t\t Parent Name: %s \t Parent id:%d\n", 
                task->comm, task->pid,
                task->parent->comm, task->parent->pid);
    }while ((task = next_task(task)) != &init_task);

    printk(KERN_INFO ">>>Cur process: name = %s\tid = %d\n", 
                current->comm, current->pid);

    return 0;
}

static void __exit md_exit(void)
{
    printk("******************************* Module md exited ************************\n");
}


module_init(md_init);
module_exit(md_exit);
