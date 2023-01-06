    include "ddraig.inc"

    section .text

startup:
__start::
    or.w    #$0700,SR                   ; Disable interrupts for now

    ; clear BSS
    lea.l   __s_bss,A0
    move.l  #__e_bss,D0

.clear_bss:
    cmp.l   D0,A0
    bge     .copy_data
    clr.w   (A0)+
    bra     .clear_bss

    ; Copy initialised data from ROM into RAM
.copy_data:
    lea.l   __e_text,A0                  ; Move data to RAM
    lea.l   __s_data,A1
    move.l  #__e_data,D0

.copy_next_word:
    cmp.l   A1,D0
    bge     .copy_vectors
    move.w  (A0)+,(A1)+
    bra     .copy_next_word

.copy_vectors:
    move.l  #vectors_end,A0             ; Start into A0 (source)
    move.l  #$400,A1                    ; 0x400 into A1 (destination)

.isr_copy_loop:
    move.l  A1,D0                       ; Have we reached destination zero?
    beq.s   .isr_copy_done              ; Halt if so

    move.l  -(A0),-(A1)                 ; Copy long source to dest, with predecrement.
    bra.s   .isr_copy_loop              ; Next iteration

.isr_copy_done:
    bsr     init_duart                  ; Initialise the MC68681
    ;lea.l   startup_message,A1
    ;jsr     print_string

    ;and.w   #$F0FF,SR                   ; Enable interrupts
    clr.l -(SP)                         ; **envp is null
    clr.l -(SP)                         ; **argv is null
    clr.l -(SP)                         ; argc is zero too
    jsr main                            ; Call the C code
    lea 12(SP),SP                       ; Clean up the stack

.infloop:
    bra .infloop                        ; main shouldn't return. loop if it does


   section .rodata

vectors_start:
    dc.l    _stack_start
    dc.l    __start

    dc.l    isr_bus_error               ; 02: Bus Error
    dc.l    isr_address_error           ; 03: Address Error
    dc.l    isr_illegal_instruction     ; 04: Illegal Instruction
    dc.l    isr_generic                 ; 05: Divide by Zero
    dc.l    isr_generic                 ; 06: CHK Instruction
    dc.l    isr_generic                 ; 07: TRAPV Instruction
    dc.l    isr_generic                 ; 08: Privilege Violation
    dc.l    isr_generic                 ; 09: Trace
    dc.l    isr_generic                 ; 0A: Line 1010 Emulator
    dc.l    isr_generic                 ; 0B: Line 1111 Emulator
    dc.l    isr_generic                 ; 0C: Reserved
    dc.l    isr_generic                 ; 0D: Reserved
    dc.l    isr_generic                 ; 0E: Format error (MC68010 Only)
    dc.l    isr_generic                 ; 0F: Uninitialized Vector

    dcb.l   8,isr_generic               ; 10-17: Reserved
    dc.l    isr_generic                 ; 18: Spurious Interrupt

    dcb.l   7,isr_generic               ; 19-1F: Level 1-7 Autovectors
    dcb.l   13,isr_generic              ; 20-2C: TRAP Handlers (unused)
    dc.l    isr_trap_13_call            ; 2D: TRAP#13 handler
    dc.l    isr_trap_14_call            ; 2E: TRAP#14 handler
    dc.l    isr_trap_15_call            ; 2F: TRAP#15 handler
    dcb.l   16,isr_generic              ; 30-3F: Remaining Reserved vectors
    dcb.l   16,isr_generic              ; 40-43: MFP GPIO #0-3 (Not used)
    dcb.l   176,isr_generic             ; 50-FF: Unused user vectors
vectors_end:

startup_message     dc.b $D,$A,'Ddraig ROM startup code completed',$D,$A,$0
