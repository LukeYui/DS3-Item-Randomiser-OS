EXTERN rItemRandomiser:QWORD
EXTERN fItemRandomiser:PROC
EXTERN qItemEquipComms:QWORD

.data

.code

tItemRandomiser PROC

	sub rsp,088h
	call _tItemRandomiser
	add rsp,088h

	Return:
	lea rbp,[rsp-027h]
	jmp [rItemRandomiser]

tItemRandomiser ENDP

_tItemRandomiser PROC

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
	mov [qItemEquipComms],rdx
	mov r9,[rbp+0110h]
	call fItemRandomiser
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

_tItemRandomiser ENDP

END