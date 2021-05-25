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
