
    section .text

isr_trap_13_call::
    rte

isr_trap_14_call::
    move.l  A0,-(SP)                     ; syscall_data
    jsr     bios_system
    lea     4(SP),SP                    ; Clean up the stack
    rte

; pointer to syscall_data in A0
; call into c function
; int bios_disk(syscall_data *sys)
isr_trap_15_call::
    move.l  A0,-(SP)                     ; syscall_data
    jsr     bios_disk
    lea     4(SP),SP                    ; Clean up the stack
    rte
