EXTERN rAutoEquip:QWORD
EXTERN fAutoEquip:PROC
EXTERN qItemEquipComms:QWORD

.data

.code

tAutoEquip PROC

	sub rsp,088h
	call _tAutoEquip
	add rsp,088h

	Return:
	pop r15
	pop r14
	pop r13
	jmp [rAutoEquip]

tAutoEquip ENDP

_tAutoEquip PROC

	mov [rsp+08h],rax
	mov [rsp+010h],rbx
	mov [rsp+018h],rcx
	mov [rsp+020h],rdx
	mov [rsp+028h],rbp
	push r8
	push r9
	push rdi
	push rsi
	sub rsp,028h
	mov rbp,rsp
	and rsp,-010h
	mov rcx,[qItemEquipComms]
	lea rdx,[rcx-010h]
	call fAutoEquip
	mov rsp,rbp
	mov rax,[rsp+050h]
	mov rbx,[rsp+058h]
	mov rcx,[rsp+060h]
	mov rdx,[rsp+068h]
	mov rbp,[rsp+070h]
	add rsp,028h
	pop rsi
	pop rdi
	pop r9
	pop r8
	ret
	ret

_tAutoEquip ENDP

END