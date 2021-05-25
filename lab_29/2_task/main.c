#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/interrupt.h>
#include <linux/workqueue.h>


#define SHARED_IRQ      1


MODULE_LICENSE("GPL");


static int irq = SHARED_IRQ, irq_counter = 0;
static int my_dev_id;

static struct workqueue_struct *my_wq;


void show_info_workqueue(struct work_struct *work);


DECLARE_WORK(my_workqueue, show_info_workqueue);

void show_info_workqueue(struct work_struct *work)
{
    printk(KERN_INFO ">>> Info about workqueue:     count = %d", irq_counter);
}

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
    irq_counter++;

    printk(KERN_INFO ">>> ISR: counter = %d", irq_counter);

    printk(KERN_INFO ">>> State before queue_work() = %s",
        my_workqueue.data);

    queue_work(my_wq, &my_workqueue); 

    printk(KERN_INFO ">>> State after queue_work()  = %s",
        my_workqueue.data);

    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    if (request_irq(irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id))
    {
        printk(KERN_INFO ">>> ERROR: request_irq failed");
        return -1;
    }

    my_wq = create_workqueue("my_queue");
    if (my_wq)
        printk(KERN_INFO ">>> Workqueue was created");
    else
    {
        free_irq(irq, &my_dev_id);
        printk(KERN_INFO ">>> Workqueue wasn't created");
        return -1;
    }

    printk(KERN_INFO "---------- Module was loaded: ISR %d ----------", irq);
    return 0;
}

static void __exit my_exit(void)
{
    flush_workqueue(my_wq);

    destroy_workqueue(my_wq);
    free_irq(irq, &my_dev_id);
    printk(KERN_INFO "---------- Module was unloaded ----------");
}


module_init(my_init);
module_exit(my_exit); 