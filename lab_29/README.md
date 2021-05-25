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

