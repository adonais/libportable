BITS 64
DEFAULT REL

%define MAX_PATH 260

global GetLoaderPic, GetLoaderPicSize

section .text
align 16

GetLoaderPicSize:
	mov rax, loader_pic_config_size + loader_pic_size
	ret

; extern "C" VOID __fastcall GetLoaderPic( PVOID picBuffer, PVOID LoadLibraryA, PCHAR dllName, DWORD dllNameLen)
GetLoaderPic:
	mov rdi, rcx
	lea rsi, [loader_pic]
	mov rcx, loader_pic_size
	rep movsb

	mov [rdi + loader_pic_config.LoadLibraryA], rdx

	lea rdi, [rdi + loader_pic_config.dllName]
	mov rsi, r8
	mov rcx, r9
	rep movsb

	ret
	
struc loader_pic_config
	.LoadLibraryA	resq	1
	.dllName		resb	MAX_PATH
endstruc

loader_pic:
	; This, children, is how we push a qword without fucking up registers in x64.
	push rax
	push rax
	mov rax, 0xDEADBEEFDEADBEEF
	mov [rsp+8h],rax
	pop rax

	push rax
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push rsi
	push rdi
	pushfq

	jmp .ldr_pic_a
.ldr_pic_b:
	pop rbx

	; Load library here ...
	mov rax, [rbx + loader_pic_config.LoadLibraryA]
	lea rcx, [rbx + loader_pic_config.dllName]

	sub rsp, 28h
	call rax
	add rsp, 28h

	popfq
	pop rdi
	pop rsi
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rbx
	pop rax
	

	; Return to normal flow of execution.
	ret
.ldr_pic_a:
	call .ldr_pic_b

loader_pic_size: equ $-loader_pic