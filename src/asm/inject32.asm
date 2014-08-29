BITS 32
%define MAX_PATH 260

global _GetLoaderPic, _GetLoaderPicSize

section .text
align 16

;extern "C" DWORD __cdecl GetLoaderPicSize();
_GetLoaderPicSize:
	mov eax, loader_pic_config_size + loader_pic_size
	ret

;extern "C" VOID __cdecl GetLoaderPic( PVOID picBuffer, PVOID LoadLibraryA, PCHAR dllName, DWORD dllNameLen);
struc GetLoaderPicArgs
	.savedEbp		resd	1
	.returnEIP		resd	1
	.picBuffer		resd	1
	.LoadLibraryA	resd	1
	.dllName		resd	1
	.dllNameLen		resd	1
endstruc
_GetLoaderPic:
	push ebp
	mov ebp, esp

	lea esi, [loader_pic]
	mov edi, [ebp + GetLoaderPicArgs.picBuffer]
	mov ecx, loader_pic_size
	rep movsb

	mov eax, [ebp + GetLoaderPicArgs.LoadLibraryA]
	mov [edi + loader_pic_config.LoadLibraryA], eax

	lea edi, [edi + loader_pic_config.dllName]
	mov esi, [ebp + GetLoaderPicArgs.dllName]
	mov ecx, [ebp + GetLoaderPicArgs.dllNameLen]
	rep movsb

	pop ebp
	ret


struc loader_pic_config
	.LoadLibraryA	resd	1
	.dllName		resb	MAX_PATH
endstruc

loader_pic:
	push 0xDEADBEEF
	pushad
	pushfd

	jmp .ldr_pic_a
.ldr_pic_b:
	pop ebx

	lea eax, [ebx + loader_pic_config.dllName]
	push eax

	mov eax, [ebx + loader_pic_config.LoadLibraryA]
	call eax

	popfd
	popad

	; Return to normal flow of execution.
	ret
.ldr_pic_a:
	call .ldr_pic_b

loader_pic_size: equ $-loader_pic