#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include <linux/uaccess.h>
#include <asm/uaccess.h>


#define MAX_COOKIE_LENGTH   PAGE_SIZE   // 4 Кб
#define PROC_FILE_NAME      "my_fortune"
#define PROC_DIR_NAME       "my_dir"
#define PROC_SLINK_NAME     "my_slink"

MODULE_LICENSE("GPL");


static struct proc_dir_entry *proc_file;    /* базовая  структура, для работы с ФС proc в ядре
                                            хранит номер inode директории, размер имени,
                                            типы и права доступа, число линков, указатель
                                            на структуру file_operations и т.д.*/
static struct proc_dir_entry *proc_slink;
static struct proc_dir_entry *proc_dir;


static char* cookie_pot;    // хранилище фортунок
static int cookie_index, next_fortune;  /* индекс первого свободного для записи элемента хранилища
                                           индекс элемента хранилища, содержащего следующую фортунку для вывода по запросу*/


// пишем в ядро
// 1ый параметр - указатель на структуру file, buff - указывает на буфер данных чтения
// count определяет размер передаваемых данных, 
// f_pos указывает на смещение от начала данных файла для операции чтения
ssize_t fortune_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    int free_space = (MAX_COOKIE_LENGTH - cookie_index) + 1;

    if (free_space < count)   // достаточно ли места для размещения фортунки
    {
        printk(KERN_ERR "my_fortune: cookie_pot is overflowed");
        return -EFAULT;
    }

/* Копирование буфера из пользовательского пространства в пространство ядра */
// 1ый аргумент - куда в пространстве ядра, 
// 2ой - откуда из пространства пользователя, 3ий - сколько байт
    if (copy_from_user(&cookie_pot[cookie_index], buff, count)) // если место есть, то строка копируется в cookie_pot
    {
        printk(KERN_INFO "my_fortune: copy_from_user error");
        return -EFAULT;
    }
    
    cookie_index += count;
    cookie_pot[cookie_index - 1] = '\n';

    cookie_pot[cookie_index] = 0;
    cookie_index += 1;

    return count;     // количество символов фактически записанных в cookie_pot
}

// читаем из ядра
// 1ый параметр - указатель на структуру file, buff - указывает на буфер данных записи
// count определяет размер передаваемых данных, 
// f_pos указывает на смещение от начала данных файла для операций записи
// (f_pos: текущая позиция чтения/записи в файле)
ssize_t fortune_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    int len;

    if (!cookie_index || *f_pos > 0) // почему смещение не должно быть больше 0
        return 0;

    if (next_fortune >= cookie_index) // если дошли до места, куда только собираемся что-то писать
        next_fortune = 0;   // для зацикливания буфера

// копирует данные из ядра в пространство пользователя
// 1ый - куда в пространстве пользователя
// 2ой - откуда в пространстве ядра
// количество копируемых байт
    len = strlen(&cookie_pot[next_fortune]);
    copy_to_user(buff, &cookie_pot[next_fortune], len);

    next_fortune += len + 1;
    *f_pos += len;

    return len;
}


static struct file_operations fops = 
{
    .owner = THIS_MODULE,
    .read = fortune_read,
    .write = fortune_write,
};


static int __init init_fortune_module(void)
{
    cookie_pot = vmalloc(MAX_COOKIE_LENGTH);    // Выделение 'виртуально' непрерывного блока памяти 
    if (!cookie_pot)
    {
        printk(KERN_INFO "my_fortune: impossible to malloc cookie_pot");
        return -ENOMEM;
    }

    memset(cookie_pot, 0, MAX_COOKIE_LENGTH);

    // создаем файл = регистрируем структуру
    // имя файла, права доступа, указатель на родительскую директорию (у нас NULL - корневой каталог /proc),
    // ук-ль на определ. нами операции на файлах (на file_operations)
    proc_file = proc_create(PROC_FILE_NAME, 0666, NULL, &fops); /* 0666 - чтение и запись всеми*/
    if (!proc_file)
    {
        vfree(cookie_pot);
        printk(KERN_INFO "my_fortune: proc_create error");
        return -ENOMEM;
    }

    proc_dir = proc_mkdir(PROC_DIR_NAME, NULL); // имя самой директории, ук-ль на proc_dir_entry (корневой каталог)
    proc_slink = proc_symlink(PROC_SLINK_NAME, NULL, PROC_FILE_NAME); //имя самой ссылки, ук-ль на proc_dir_entry (корневой каталог), имя файла на который она указывает

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
    // удаляем созданные структуры
    remove_proc_entry(PROC_FILE_NAME, NULL); // имя и корневой каталог
    remove_proc_entry(PROC_DIR_NAME, NULL);
    remove_proc_entry(PROC_SLINK_NAME, NULL);

    if (cookie_pot)
        vfree(cookie_pot); // Освобождение блока памяти, выделенного функцией vmalloc

    printk(KERN_INFO "===== Fortune module unloaded! =====");
}



module_init(init_fortune_module); // макросы, инициализирующие...
module_exit(exit_fortune_module);