#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>


#define SHARED_IRQ      1


static int irq = SHARED_IRQ, irq_counter = 0;
static int my_dev_id;
char data[] = "Use IRQ 1 - keyboard";


MODULE_LICENSE("GPL");

void show_info_tasklet(unsigned long data);

//DECLARE_TASKLET(my_tasklet, show_info_tasklet, (unsigned long) &data);
struct tasklet_struct my_tasklet;   //

void show_info_tasklet(unsigned long data)
{
    printk(KERN_INFO ">>> Info about tasklet:       state = %ld, count = %d, data = %s",
        my_tasklet.state, my_tasklet.count.counter, (char *)data);
}

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
    irq_counter++;

    printk(KERN_INFO ">>> ISR: counter = %d", irq_counter);

    printk(KERN_INFO ">>> State before tasklet_schedule() = %ld",
        my_tasklet.state);

    tasklet_schedule(&my_tasklet);

    printk(KERN_INFO ">>> State after tasklet_schedule()  = %ld",
        my_tasklet.state);

    return IRQ_HANDLED;
}

static int __init my_init(void)
{
    tasklet_init(&my_tasklet, show_info_tasklet, (unsigned long) &data);    //

    if (request_irq(irq, my_interrupt, IRQF_SHARED, "my_int_keyboard_tasklet", &my_dev_id))
    {
        printk(KERN_INFO ">>> ERROR: request_irq failed");
        return -1;
    }

    printk(KERN_INFO "---------- Module was loaded: ISR %d ----------", irq);
    return 0;
}

static void __exit my_exit(void)
{
    synchronize_irq(irq);
    free_irq(irq, &my_dev_id);
    tasklet_kill(&my_tasklet);
    printk(KERN_INFO "---------- Module was unloaded ----------");
}


module_init(my_init);
module_exit(my_exit); 