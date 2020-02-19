EXTERN rNoWeaponRequirements:QWORD
EXTERN fNoWeaponRequirements:PROC

.data

.code

tNoWeaponRequirements PROC

	sub rsp,088h
	call _tNoWeaponRequirements
	add rsp,088h

	Return:
	movzx eax,byte ptr [rax+0EFh]
	jmp [rNoWeaponRequirements]

tNoWeaponRequirements ENDP

_tNoWeaponRequirements PROC

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
	lea rcx,[rax+0EEh]
	call fNoWeaponRequirements
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

_tNoWeaponRequirements ENDP

END