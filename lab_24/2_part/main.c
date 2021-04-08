#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include <linux/uaccess.h>
#include <asm/uaccess.h>


#define MAX_COOKIE_LENGTH   PAGE_SIZE
#define PROC_FILE_NAME      "my_fortune"
#define PROC_FILE_NAME_2    "my_fortune_2"
#define PROC_DIR_NAME       "my_dir"
#define PROC_SLINK_NAME     "my_slink"

MODULE_LICENSE("GPL");


static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_file_2;
static struct proc_dir_entry *proc_slink;
static struct proc_dir_entry *proc_dir;


static char* cookie_pot;
static int cookie_index, next_fortune;


ssize_t fortune_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    int free_space = (MAX_COOKIE_LENGTH - cookie_index) + 1;

    if (free_space < count)
    {
        printk(KERN_ERR "my_fortune: cookie_pot is overflowed");
        return -EFAULT;
    }

    if (copy_from_user(&cookie_pot[cookie_index], buff, count))
    {
        printk(KERN_INFO "my_fortune: copy_from_user error");
        return -EFAULT;
    }
    
    cookie_index += count;
    cookie_pot[cookie_index - 1] = '\n';

    cookie_pot[cookie_index] = 0;
    cookie_index += 1;

    return count;
}


ssize_t fortune_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    int len;

    if (!cookie_index || *f_pos > 0)
        return 0;

    if (next_fortune >= cookie_index)
        next_fortune = 0; 

    len = strlen(&cookie_pot[next_fortune]);
    copy_to_user(buff, &cookie_pot[next_fortune], len);

    next_fortune += len + 1;
    *f_pos += len;

    return len;
}

int my_open(struct inode *ind, struct file *filp)
{
    printk(KERN_INFO "***** File was opened! *****");

    return 0;
}


static struct file_operations fops = 
{
    .owner = THIS_MODULE,
    .read = fortune_read,
    .write = fortune_write,
    .open = my_open,
};


static int __init init_fortune_module(void)
{
    cookie_pot = vmalloc(MAX_COOKIE_LENGTH);
    if (!cookie_pot)
    {
        printk(KERN_INFO "my_fortune: impossible to malloc cookie_pot");
        return -ENOMEM;
    }

    memset(cookie_pot, 0, MAX_COOKIE_LENGTH);

    proc_file = proc_create(PROC_FILE_NAME, 0666, NULL, &fops);
    if (!proc_file)
    {
        vfree(cookie_pot);
        printk(KERN_INFO "my_fortune: proc_create error");
        return -ENOMEM;
    }

    proc_file_2 = proc_create(PROC_FILE_NAME_2, 0666, NULL, &fops);

    proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
    proc_slink = proc_symlink(PROC_SLINK_NAME, NULL, PROC_FILE_NAME);

    if (!proc_dir || !proc_slink)
    {
        vfree(cookie_pot);
        printk(KERN_INFO "my_fortune: proc_mkdir/proc_slink error");
        return -ENOMEM;
    }

    cookie_index = 0;
    next_fortune = 0;

    printk(KERN_INFO "===== Fortune module loaded! =====");

    return 0;
}

static void __exit exit_fortune_module(void)
{
    remove_proc_entry(PROC_FILE_NAME, NULL);
    remove_proc_entry(PROC_FILE_NAME_2, NULL);
    remove_proc_entry(PROC_DIR_NAME, NULL);
    remove_proc_entry(PROC_SLINK_NAME, NULL);

    if (cookie_pot)
        vfree(cookie_pot);

    printk(KERN_INFO "===== Fortune module unloaded! =====");
}



module_init(init_fortune_module);
module_exit(exit_fortune_module);