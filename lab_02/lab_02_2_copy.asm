.386P

descr	struc			
	limit		dw	0
	base_1 		dw	0			
	base_m		db	0			
	attr_1		db	0			
	attr_2		db 	0			
	base_h		db	0			
descr	ends 

descr_inter	struc			
	offs_l		dw	0
	sel 		dw	40			
	rsrv		db	0			
	attr		db	0			
	offs_h		dw 	0					
descr_inter	ends			

data	segment	'data' use16
	gdt_label	label
	gdt_null 	descr <0, 0, 0, 0, 0, 0>	
	gdt_data	descr <data_size-1, 0, 0, 92h>		; Селектор 8, сегмент данных
	gdt_code	descr <code_size-1,,, 98h>			; Селектор 16, сегмент команд
	gdt_stack	descr <255, 0, 0, 92h, 40h>		  	; Селектор 24, сегмент стека 
	gdt_screen	descr <4095, 8000h, 0Bh, 92h>		; Селектор 32, видеобуфер
	
	gdt_code_32 descr <code_32_size - 1, 0, 0, 98h, 40h, 0>	; Селектор 40
	gdt_data_32	descr <0FFFFh, 0, 0, 92h, 0CFh, 0>			; Селектор 48
	
	gdt_size=$-gdt_label
	
	
	idt_label	label
	descr_inter 13 	dup (<0, 40, 0, 8Fh, 0>)
	descr_inter <execute_13_off, 40, 0, 8Fh, 0>
	descr_inter 18 	dup (<0, 40, 0, 8Fh, 0>)
	
	idt_32 		descr_inter <int_32_off, 40, 0, 8Eh, 0>
	idt_33		descr_inter <int_33_off, 40, 0, 8Eh, 0>
	
	idt_size=$-idt_label
	
	pdescr				dq	0
	msg_before_prot		db	27,'[31;42m Ready to work in protected mode!',27,'[0m$' 
	end_of_msg			db 	13, 10, 13, 10, 13, 10, '$'
	msg_prot			db	'Moved to protected mode!$'
	msg_real			db	13, 10, 27,'[31;42m Return to real mode!	',27,'[0m$'
	
	sym_table			db  0, 27, '1234567890-=  '
						db	'qwertyuiop[]  '
						db  'asdfghjkl;    '
						db  'zxcvbnm,./'
						db  50h dup(0)
	
	master_Mask			db 0
	slave_Mask			db 0
	
	int_timer_pos		= (75) * 2
	int_keyboard_pos	dw 	(80*24) * 2
	
	int_timer_count		dd	0
	
	sym_color			db 00011111b
	
	return_flag			db	0		; Флаг выхода из защищённого режима
	
	msg_memory			db	'Size of available memory: '
	msg_memory_add		db	'           bytes$'
	
	memory_pos			=	80*23
	
	data_size=$-gdt_null
data	ends
	
show_msg	macro msg:REQ, pos
	local	loop_label
	mov		ESI, pos * 2
	mov		EDI, 0
	mov		ECX, 0
	loop_label:
			mov 	AH, 00011111b
			mov 	AL, &msg[DI]
			mov 	ES:[ESI], AX
			add 	ESI, 2
			inc 	EDI
			
			cmp		msg[DI], '$'
	loopne loop_label
endm
	
code_32 	segment 'code' use32
			assume CS:code_32, DS:data
code_32_begin	label

execute_code	proc
	iretd
execute_code	endp

execute_13_off = $-execute_code
execute_13 proc
	pop		EAX
	pop		EAX
	iretd
execute_13 endp

mb_size = 1024 * 1024 + 1
show_memory proc
	push	ES
	cli
	
	mov		AX, 48
	mov 	ES, AX
	
	mov 	EBX, mb_size
	mov 	ECX, -mb_size
	
	memory_loop:
		mov		AH, ES:[EBX]
		xor		byte ptr ES:[EBX], 0FFh
		cmp 	AH, ES:[EBX]
		je 		memory_exit
		mov		ES:[EBX], AH
		inc 	EBX
	loop 	memory_loop
	
memory_exit:
	pop 	ES
	sti
	
	mov 	EAX, EBX
	mov 	EBX, 10
	mov		EDI, 9
	mov 	ECX, 10
	fill_msg_memory:
		xor		EDX, EDX
		div 	EBX
		add 	DL, '0'
		mov 	msg_memory_add[EDI], DL
		dec 	EDI
	loop 	fill_msg_memory
	
	show_msg DS:msg_memory, memory_pos
	ret
show_memory endp

; Прерывание от системного таймера
temp	dw	10
show_amount_ticks proc
	mov		BX, int_timer_pos
	mov		EAX, int_timer_count
	mov		ECX, 0
	
	calculate_loop:
		xor 	EDX, EDX
		div		temp
		
		mov 	DH, 00011111b
		add		DL, '0'
		mov		ES:[BX], DX
		
		sub		BX, 2
		cmp		EAX, 0
	loopne	calculate_loop
	ret
show_amount_ticks endp

int_32_off = $-execute_code
int_timer_32 proc
	pushad
	
	call 	show_amount_ticks
	inc		int_timer_count
	
	mov		AL, 20h		; EOI
	out		20h, AL
	popad
	iretd
int_timer_32 endp

process_code proc
	in		AL, 60h			; Считывание скан-кода с клавиатуры
	mov		AH, AL			
	in		AL, 61h			; Получение содержимого порта B
	or		AL, 80h
	out		61h, AL			
	and		AL, 7Fh			
	out		61h, AL			; Сообщение контроллеру о приёме скан-кода 
	mov		AL, AH			; Востановление скан-кода
	ret
process_code endp

int_33_off = $-execute_code
; Прерывание от клавиатуры
int_keyboard_33 proc
	pushad
	
	call 	process_code
	
	cmp 	AL, 1Ch				; Enter
	je		set_return
	
	cmp		AL, 0Eh				; BackSpace
	je		dec_pos_33
	
	test 	AL, 80h
	jnz		exit
	
	lea		EBX, sym_table
	xlat					; Получение символа по скан-коду
	
	mov		AH, sym_color
	mov 	BX, int_keyboard_pos
	mov		ES:[BX], AX
	
	add		int_keyboard_pos, 2
	cmp		int_keyboard_pos, 80*25*2
	jb 		exit
	
	mov		int_keyboard_pos, 80*24*2
	jmp		exit
	
dec_pos_33:
	cmp		int_keyboard_pos, 80*24*2
	jbe		exit
	
	sub		int_keyboard_pos, 2
	mov		AL, 0
	mov		AH, 00h
	mov 	BX, int_keyboard_pos
	mov		ES:[BX], AX
	
	jmp		exit
	
set_return:
	mov 	return_flag, 1
	
exit:
	mov		AL, 20h		; EOI
	out		20h, AL
	popad
	iretd
int_keyboard_33 endp



protected_mode:
	mov 	AX,8				; Селектор сегмент данных
	mov		DS,AX				
	mov 	AX,24				; Селектор сегмента стека
	mov		SS,AX
	mov		AX,32				; Селектор сегмента видеобуфера
	mov		ES,AX		
	
	sti
	call 	show_memory
	show_msg	msg_prot, 80*22
	
	waiting:
		hlt 
		cmp 	return_flag, 1
	jne	waiting
	
	cli
	
	
	db		0EAh				
	dd		offset go			; загрузим теневой регистр
	dw		16					; сегмента команд
	
code_32_size = $-code_32_begin
code_32 ends
	
fill_descr macro segm_label, desc:REQ
	xor		EAX,EAX			
	mov 	AX,segm_label			
	shl 	EAX,4				; В EAX линейный базовый адрес
	mov 	BX, offset &desc			
	mov 	[BX].base_1,AX		
	rol		EAX,16				; Обмен старшей и младшей половин EAX
	mov 	[BX].base_m,AL	
endm
	
fill_descr_inter macro inter_label, desc:REQ
	mov 	EAX, offset inter_label
	mov 	&desc.offs_l, AX
	shr		EAX, 16
	mov 	&desc.offs_h, AX
endm

text	segment 'code'	use16	
		assume CS:text, DS:data
text_begin 	label
		
main	proc					
		xor		EAX,EAX			
		mov 	AX,data				; Загрузим в DS сегментный
		mov		DS,AX				; адрес сегмента данных
;Вычислим 32-битовый линейный адрес сегмента данных и загрузим его
;в дескриптор сегмента данных в GDT.
		shl 	EAX,4				; В EAX линейный базовый адрес
		mov 	EBP,EAX	

		fill_descr	DS, gdt_data
		fill_descr  CS, gdt_code
		fill_descr	SS, gdt_stack
		fill_descr	code_32, gdt_code_32
				
		;mov 	CX, 32
		;mov 	BX, 0
		;execute_loop:
		;	fill_descr_inter execute_code idt_label[BX]
		;	inc		BX
		;loop 	execute_loop
		
		;fill_descr_inter	execute_13 idt_label[13]
		;fill_descr_inter	int_timer_32, idt_32
		;fill_descr_inter	int_keyboard_33, idt_33
		
		mov		AH, 09h
		mov		DX, offset msg_before_prot
		int 	21h
		mov		AH, 09h
		mov		DX, offset end_of_msg
		int 	21h
		
;Подготовим псевдодескриптор pdescr и загрузим регистр GDTR
		mov		dword ptr pdescr+2, EBP
		mov 	word ptr pdescr, gdt_size-1
		lgdt	pdescr	
		
;Сохранение маски прерываний
		in		AL, 21h
		mov		master_Mask, AL
		in		AL, 0A1h
		mov		slave_Mask, AL
		
;Открытие линии А20
		in		AL, 92h		
		or		AL, 10b
		out		92h, AL
	
;Изменение базового вектора ведущего контроллера
		cli
		mov		AL, 11h
		out		20h, AL
		mov		AL, 33
		out		21h, AL
		mov		AL, 4
		out		21h, AL
		mov		AL, 1
		out		21h, AL
	
;Установление новой маски
		mov		AL, 0FCh		; Разрешается только прерывание от таймера и клавиатуры
		out		21h, AL
		
		mov		AL, 0FFh		; Запрет всех прерываний ведомого контроллера
		out		0A1h, AL
		
;Загрузка IDTR
		mov 	word ptr pdescr, idt_size-1
		xor		EAX, EAX
		mov 	AX, offset idt_label
		add 	EAX, EBP				; Адрес idt
		mov 	dword ptr pdescr+2,EAX	
		lidt	pdescr
		
;Подготовимся к переводу в защищенный режим						
		mov		AL,80h				; Запрет NMI
		out 	70h,AL				; Порт КМОП-микросхемы
		
;Переходим в защищенный режим
		mov 	EAX,CR0				
		or		EAX,1				; Установим бит PE
		mov		CR0,EAX				; Запишем назад в CR0
		
;Теперь процессор работает в защищённом режиме
		db 		66h					; Префикс изменения разрядности операнда
		db 		0EAh				; Код команды far jmp
		dd 		offset protected_mode		
		dw		40					; Селектор code_32
		
go:		mov 	EAX,CR0				
		and 	EAX,0FFFFFFFEh		
		mov		CR0,EAX				
		db		0EAh				
		dw		offset return		
		dw		text				
;Теперь процессор снова работает в реальном режиме
;Восстановим операционную среду реального режима
return: mov		AX,data				; Восстановим
		mov		DS,AX				; адресуемость данных
		mov		AX,stk32			; Восстановим
		mov		SS,AX				; адресуемость стека
		mov 	SP, 256
		
; Востановление IDTR
		mov		AX, 3FFh 			; Граница таблицы векторов	
		mov		word ptr pdescr, AX
		mov		EAX, 0
		mov		dword ptr pdescr+2, EAX
		lidt	pdescr
		
; Реинициализирование ведущего контроллера на базовый вектор 8
		mov		AL, 11h
		out		20h, AL
		mov		AL, 8
		out		21h, AL
		mov		AL, 4
		out		21h, AL
		mov		AL, 1
		out		21h, AL
		
; Востановление исходных масок
		mov		AL, master_Mask
		out		21h, AL
		mov		AL, slave_Mask
		out		0A1h, AL
		
; Закрытие линии А20
		in		AL, 92h		
		and		AL, 0FDh
		out		92h, AL
		
; Разрешим маскируемые и немаскируемые прерывания
		sti							
		mov		AL,0				; Сброс бита 7 в порте CMOS - 
		out		70h,AL				; - разрешение NMI
; Проверим выполнение функций DOS после возврата в реальный режим
		mov 	AH,09h				
		mov		DX,offset msg_real		
		int 	21h					
		
		mov		AX,4C00h			; Завершим программу
		int		21h					; обычным образом
		main endp			
code_size=$-text_begin				
text	ends
						
stk32	segment stack 'stack' use32	
		db		256 dup ('^')		
stk32	ends						
		end main				
