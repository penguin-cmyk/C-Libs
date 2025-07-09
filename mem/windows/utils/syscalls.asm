; nasm 
%macro SYSCALL_STUB 2
global %1
section .text
%1:
    mov r10, rcx
    mov eax, %2
    syscall
    ret
%endmacro

SYSCALL_STUB NtReadVirtualMemoryEx, 0x177 ;
SYSCALL_STUB NtWriteVirtualMemory, 0x3A ;
SYSCALL_STUB NtOpenProcess, 0x26 ;
SYSCALL_STUB NtAllocateVirtualMemoryEx, 0x78 ;

;SYSCALL_STUB ZwFreeVirtualMemory, 0x1e
;SYSCALL_STUB ZwProtectVirtualMemory, 0x50
;SYSCALL_STUB NtCreateThreadEx, 0xc9
SYSCALL_STUB NtClose, 0xf
SYSCALL_STUB ZwQueryVirtualMemory, 0x23 ;
