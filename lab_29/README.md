# Системный вызов
- интерфейс, который ОС предоставляет в распоряжение пользователя (т.е. набор функций)
- запрос сервиса ОС (API)

## Что происходит при системном вызове?
1. программа прерывается
2. сохраняется аппаратный контекст (сейчас в регистрах, раньше в стеке) - чтобы затем продолжить работу  

# Прерывание
— сигнал от программного или аппаратного обеспечения, сообщающий процессору о наступлении какого-либо события, требующего немедленного внимания. Прерывание извещает процессор о наступлении высокоприоритетного события,
требующего прерывания текущего кода, выполняемого процессором.  
Процессор отвечает приостановкой своей текущей активности, сохраняя свое состояние и выполняя функцию,
называемую обработчиком прерывания, которая реагирует на событие и обслуживает его, после чего возвращает управление в прерванный код.

# 3 типа прерываний
1. системные вызовы (вызов API - те функции, который ОС предоставляет в распоряжение пользователю, чтобы он мог запросить сервис системы)
2. исключения (те исключительные ситуации, которые система может перехватить)
3. аппаратные прерывания   
3.1.  прерывания от системного таймера   
3.2.  от внешних устройств (для информирования процессора о завершении операции I/O)  

1-2 - **синхронные** = возникают в процессе выполнения программы (внутренние)  
3 - **ассинхронные** = не зависят ни от каких действий в системе (от внешних устройств)

> Внешние устройства управляются специальными устройствами:  
> **Контроллер** - входит в состав внешнего устройства  
> **Адаптер** - находится на материнской плате  

# Аппаратные прерывания
1. быстрые - обработчик которых выполняется от начала до конца. Осталось только одно такое - обработчик системного таймера
2. медленные  
2.1. собственное обработчик прерывания (**top-half**)  
2.2. отложенное действие (**bottom-half**)  
2.2.1. softirq (гибкие прерывания) - могут выполняться параллельно на разных процессорах, нужны жесткие требования к взаимоисключениям  
2.2.2. tasklets - не могут выполняться параллельно = отложенное действие    
2.2.3. work queue (очереди работ)  


# Как обрабатываются сигналы прерываний
Сигналы прерывания от устройств I/O поступают на входы IRQ (Interrupt Request), а контроллер прерывания формирует сигнал прерывания, который по шине управления
(линии INTR) поступает на соответствующую ножку (pin) процессора.  

Сигнал прерывания будет передан процессору, если он не замаскирован, т.е.
его обработка разрешена.  

Для увеличения числа обрабатываемых
прерываний контроллеры стали подключать в виде каскада: ведущий и
ведомый контроллеры (всего 15 линий IRQ, одна линия используется для
каскадного соединения).

# Раскладка прерываний
**IRQ 0 — system timer**  
**IRQ 1 — keyboard controller**  
IRQ 2 — cascade (прерывание от slave контроллера)  
IRQ 3 — serial port COM2  
IRQ 4 — serial port COM1  
IRQ 5 — parallel port 2 and 3 or sound card  
IRQ 6 — floppy controller  
IRQ 7 — parallel port 1  
IRQ 8 — RTC timer  
IRQ 9 — ACPI  
IRQ 10 — open/SCSI/NIC  
IRQ 11 — open/SCSI/NIC  
**IRQ 12 — mouse controller**  
IRQ 13 — math co-processor  
IRQ 14 — ATA channel 1  
IRQ 15 — ATA channel 2  

![изображение](https://user-images.githubusercontent.com/54107546/119554116-fc4b1f80-bda4-11eb-8bf3-6779853688f7.png)

1. Первая колонка: номер прерывания
2. Колонки CPUx: счётчики прерываний на каждом из процессоров
3. Следующая колонка: вид прерывания:  
3.1  IO-APIC-edge — прерывание по фронту на контроллер I/O APIC  
3.2  IO-APIC-fasteoi — прерывание по уровню на контроллер I/O APIC  
3.3  PCI-MSI-edge — MSI прерывание  
3.4  XT-PIC-XT-PIC — прерывание на PIC контроллер  
4. Последняя колонка: устройство, ассоциированное с данным прерыванием

---

# ЛАБОРАТОРНАЯ РАБОТА
# 1 программа

## int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char* name, void * dev);
- регистрирует на определённой линии IRQ обработчик аппаратного прерывания
- параметры:
  - irq – номер линии прерывания, на которую регистрируется handler (0 - таймер, 1 - клава, 12 - мышь)
  - handler – обработчик, указатель на актуальный обработчик прерывания
  - irqflags – флаги
  > В нашем случае, **IRQF_SHARED** = указывает, что линия IRQ может разделяться (совместное использование линии)
  - name – ASCII текст, то есть имя устройства, связанного с прерыванием
  - dev – уникальный указатель экземпляра обработчика, который передается этой функции при его регистрации.

## ISR = Interrupt Service Routine - другое название handler

## void free_irq(unsigned int irq, void * dev);
- void * dev - чтобы мы могли освободить линию IRQ от того, что мы на неё повесили (передаем указатель на IRQ-handler)

## void synchronize_irq(unsigned int irq);
- ожидает пока завершится обработчик прерывания от линии IRQ (если он выполняется)
> хорошая идея — всегда вызывать эту функцию перед выгрузкой модуля использующего эту линию IRQ; @Цилюрик
> НО ВНИМАНИЕ: If you use this function while holding a resource the IRQ handler may need you will **deadlock**. 

## struct tasklet_struct
```c
struct tasklet_struct
{
  struct tasklet_struct *next; /* указатель на следующий тасклет в списке тасклетов */
  unsigned long state; /* состояние тасклета */
  atomic_t count; /* счетчик ссылок */
  void (*func) (unsigned long); /* функция-обработчик тасклета*/
  unsigned long data; /* аргумент функции-обработчика тасклета */
);
```

**state**:
* 0
* TASKLET_STATE_SCHED = 1 - тасклет запланирован на выполнение
* TASLET_STATE_RUN = 2 - тасклет выполняется

**count**:
* 0 - тасклет разрешен и может выполняться в случае, когда он помечен как ожидающий выполнения
* не 0 - тасклет запрещен и не может выполняться

## tasklet_schedule(struct tasklet_struct * t);
- планируется тасклет
> * Когда tasklet запланирован, ему выставляется состояние **TASKLET_STATE_SCHED**, и он **добавляется в очередь**. 
> * Пока он находится в этом состоянии,запланировать его еще раз не получится — ничего не произойдет. 
> * Tasklet **не** может находиться сразу в нескольких местах в очереди на планирование, которая организуется через поле next структуры tasklet_struct.
> * После того, как тасклет был запланирован, он выполниться **один раз**.
> * Тасклет выполняется на том процессоре, на котором выполнялся обработчик прерывания 
> * Только один тасклет данного типа выполняется в любой момент времени (но тасклеты разных типов могут выполняться одновременно)

## IRQ_HANDLED
— устройство прерывания распознано как обслуживаемое обработчиком, и прерывание успешно обработано. 

## tasklet_kill (struct tasklet_struct * t)
- ждет завершения тасклета и удаляет тасклет из очереди на выполнение только в контексте процесса.

> Причем, убит он будет только уже запланирован.

## Регистрация тасклетов
### Статически
С помощью макросов:
1. DECLARE_TASKLET(name, func, data)
2. DECLARE_TASKLET_DISABLED(name, func, data);

**Результат:**   
экземпляр структуры **struct tasklet_struct**  
### Динамически
С помощью функции:  
## tasklet_init(t, tasklet_handler, dev);  
где t - указатель на структуру struct tasklet_struct

---

# 2 программа
## Очереди работ
* Вместо того, чтобы предлагать однократную схему отложенного исполнения, как в случае с тасклетами, очереди работ являются обобщенным механизмом отложенного исполнения, в котором функция обработчика, используемая для очереди работ, может "засыпать" (что невозможно в модели тасклетов).
* Tasklet'ы разгребаются функцией-планировщиком, а workqueue обрабатывается специальными потоками, которые зовутся worker'ами.

## struct workqueue_struct * create_workqueue(name);
- создаётся очередь работ  

## void destroy_workqueue(struct workqueue_struct * );
- удалить очередь работ

## int flush_workqueue(struct workqueue_struct * wq);
- завершить все работы в данной очереди

## int queue_work(struct workqueue_struct * wq, struct work_struct * work);
- добавить работу в очередь работ

## struct workqueue_struct
```c
struct workqueue_struct {
	struct list_head	pwqs;		/* WR: all pwqs of this wq */
	struct list_head	list;		/* PR: list of all workqueues */

	struct mutex		mutex;		/* protects this wq */
	int			work_color;	/* WQ: current work color */
	int			flush_color;	/* WQ: current flush color */
	atomic_t		nr_pwqs_to_flush; /* flush in progress */
	struct wq_flusher	*first_flusher;	/* WQ: first flusher */
	struct list_head	flusher_queue;	/* WQ: flush waiters */
	struct list_head	flusher_overflow; /* WQ: flush overflow list */

	struct list_head	maydays;	/* MD: pwqs requesting rescue */
	struct worker		*rescuer;	/* MD: rescue worker */

	int			nr_drainers;	/* WQ: drain in progress */
	int			saved_max_active; /* WQ: saved pwq max_active */

	struct workqueue_attrs	*unbound_attrs;	/* PW: only for unbound wqs */
	struct pool_workqueue	*dfl_pwq;	/* PW: only for unbound wqs */

#ifdef CONFIG_SYSFS
	struct wq_device	*wq_dev;	/* I: for sysfs interface */
#endif
#ifdef CONFIG_LOCKDEP
	char			*lock_name;
	struct lock_class_key	key;
	struct lockdep_map	lockdep_map;
#endif
	char			name[WQ_NAME_LEN]; /* I: workqueue name */

	/*
	 * Destruction of workqueue_struct is RCU protected to allow walking
	 * the workqueues list without grabbing wq_pool_mutex.
	 * This is used to dump all workqueues from sysrq.
	 */
	struct rcu_head		rcu;

	/* hot fields used during command issue, aligned to cacheline */
	unsigned int		flags ____cacheline_aligned; /* WQ: WQ_* flags */
	struct pool_workqueue __percpu *cpu_pwqs; /* I: per-cpu pwqs */
	struct pool_workqueue __rcu *numa_pwq_tbl[]; /* PWR: unbound pwqs indexed by node */
};
```

## struct work_struct;
- задача в очереди работ  
```c
struct work_struct {
	atomic_long_t data;
	struct list_head entry;
	work_func_t func;
#ifdef CONFIG_LOCKDEP
	struct lockdep_map lockdep_map;
#endif
};
```
## Поместить задачу в очередь работ можно:
### Статически
С помощью макроса:
DECLARE_WORK(name, void (* func)(void * ));
- name – имя структуры work_struct, 
- func – обработчик нижней половины.

### Динамически
С помощью 2 макросов:
1. INIT_WORK(sruct work_struct * work, void (* func)(void),void * data);
2. PREPARE_WORK(sruct work_struct * work, void (* func)(void),void * data);
