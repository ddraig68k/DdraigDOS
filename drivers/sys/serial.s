    include 'ddraig.inc'

DUART_MR1A      EQU     $00             ; Mode Register A
DUART_MR2A      EQU     $00             ; Mode Register A
DUART_SRA       EQU     $02             ; Status Register A
DUART_CSRA      EQU     $02             ; Clock-Select Register A
DUART_CRA       EQU     $04             ; Command Register A
DUART_RBA       EQU     $06             ; Receive Buffer A
DUART_TBA       EQU     $06             ; Transmit Buffer A
DUART_IPCR      EQU     $08             ; Input Port Change Register
DUART_ACR       EQU     $08             ; Auxiliary Control Register
DUART_ISR       EQU     $0A             ; Interrupt Status Register
DUART_IMR       EQU     $0A             ; Interrupt Mask Register
DUART_CUR       EQU     $0C             ; Counter Mode: current MSB
DUART_CTUR      EQU     $0C             ; Counter/Timer upper reg
DUART_CLR       EQU     $0E             ; Counter Mode: current LSB
DUART_CTLR      EQU     $0E             ; Counter/Timer lower reg
DUART_MR1B      EQU     $10             ; Mode Register B
DUART_MR2B      EQU     $10             ; Mode Register B
DUART_SRB       EQU     $12             ; Status Register B
DUART_CSRB      EQU     $12             ; Clock-Select Register B
DUART_CRB       EQU     $14             ; Command Register B
DUART_RBB       EQU     $16             ; Receive Buffer B
DUART_TBB       EQU     $16             ; Transmit Buffer A
DUART_IVR       EQU     $18             ; Interrupt Vector Register
DUART_IP        EQU     $1A             ; Input Port
DUART_OPCR      EQU     $1A             ; Output Port Configuration Reg.
DUART_STRTCC    EQU     $1C             ; Start-Counter command
DUART_OPRSET    EQU     $1C             ; Output Port Reg,SET bits
DUART_STOPCC    EQU     $1E             ; Stop-Counter command
DUART_OPRRST    EQU     $1E             ; Output Port Reg,ReSeT bits

; Status Register
RxRDY		EQU     $00
TxRDY		EQU     $02


    section .text

init_duart::
      move.l A0,-(SP)                     ; save registers
      lea DUART_BASE,A0
      move.b #$20,DUART_CRA(A0)           ; reset receiver
      move.b #$10,DUART_CRA(A0)           ; reset MR pointer to MRx1

      move.b #$00,DUART_ACR(A0)           ; select BAUD rate set 1
      move.b #$CC,DUART_CSRA(A0)          ; set TX & RX to 38400 BAUD
      move.b #$93,DUART_MR1A(A0)          ; set 8bits,no parity,character mode,RxRDY IRQ,RxRTS enabled
      move.b #$27,DUART_MR2A(A0)          ; $37 Normal Mode,enable TxRTS & TxCTS,one Stop bit
      move.b #$05,DUART_CRA(A0)           ; enable Receiver & Transmitter

      move.b #$BB,DUART_CSRB(A0)          ; set TX & RX to 9600 BAUD
      move.b #$93,DUART_MR1B(A0)          ; set 8bits,no parity,character mode,RxRDY IRQ,RxRTS enabled
      move.b #$27,DUART_MR2B(A0)          ; $37 Normal Mode,enable TxRTS & TxCTS,one Stop bit
      move.b #$05,DUART_CRB(A0)           ; enable Receiver & Transmitter

      move.b #$05,DUART_OPRSET(A0)        ; $01 assert rts
      move.l (SP)+,A0                     ; restore registers
      rts

char_out::
      movem.l A0/D1,-(SP)                 ; save A0/D1
      lea DUART_BASE,A0
.coutpoll
      move.b DUART_SRA(A0),D1             ; wait for OK to transmit
      btst #TxRDY,D1
      beq .coutpoll

      move.b D0,DUART_TBA(A0)             ; send Char

      movem.l (SP)+,A0/D1                 ; restore A0/D1
      rts

; read character with no echo
char_ins::
      movem.l A0/D1,-(SP)                 ; save registers
      lea DUART_BASE,A0
.cinspoll
      move.b DUART_SRA(A0),D1
      btst #RxRDY,D1
      beq .cinspoll
      move.b DUART_RBA(A0),D0
      movem.l (SP)+,A0/D1                 ; restore registers
      rts

char_in:
      bsr char_ins
      bsr char_out
      rts

char_in_ready:
    btst #RxRDY,(DUART_BASE+DUART_SRA)
    rts

serial_has_char::
    moveq #0,D0
    bsr char_in_ready
    beq .nochar
    moveq #1,D0
.nochar:
    rts


read_serial_inputs::
    move.b (DUART_BASE+DUART_IP),D0
    rts



; A1 Address of the string to print
; END WITH 0
print_string::
      move.b (A1)+,D0
      cmp.b  #0,D0
      beq.s .pstr_end
      bsr char_out
      bra.s print_string
.pstr_end
      rts

; NEW LINE
print_newline::
      move.l D0,-(SP)
      move.b #$D,D0
      bsr char_out
      move.b #$A,D0
      bsr char_out
      move.l (SP)+,D0
      rts

print_space::
      move.b #' ',D0
      bsr char_out
      rts


    ; putchar function called by printf
putchar_serial::
_putchar_serial::
    move.b  7(SP),D0                ; Get C char from the stack into D0
    bsr     char_out                ; Send the character
    cmp.b   #$0A,D0
    bne     .endputchar
    move.b  #$0D,D0                 ; If sending LF send a CR as well
    bsr     char_out
.endputchar
    rts

puts_unused::
    move.l  4(SP),A1                ; Get C char pointer from the stack into A1
    bsr     print_string            ; Send the string
    rts

serial_get::
    bsr     char_ins                ; get the character from the serial ports
    rts

serial_write_old::
    tst.l   8(SP)
    beq     .len_zero
    move.l  4(SP),A0
    move.l  8(SP),D1
.send_char:
    move.b  (A0)+,D0
    bsr     char_out
    subq.l  #1,D0
    bne .send_char

.len_zero:
    rts

serial_write::
        movem.l A2/D1-D2,-(SP)
        move.l 16(SP),A2
        tst.l 20(SP)
        beq .len_zero
        move.l 20(SP),D2
        add.l A2,D2
.next_char:
        move.b (A2)+,D0
        bsr char_out
        cmp.l D2,A2
        bne .next_char
.len_zero:
        clr.l D0
        movem.l (SP)+,A2/D1-D2
        rts


; PRINT HEX
; print_1hex = PRINT ONE HEX
; print_2hex = PRINT TWO
; print_4hex = PRINT FOUR
; print_8hex = PRINT EIGHT
; ENTRY: D0
print_1hex::
      move.b D0,-(SP)                     ;SAVE D0
      and.b #$F,D0
      add.b #'0',D0
      cmp.b #'9',D0
      bls.s   .print_1hex
      add.b #7,D0
.print_1hex
      bsr char_out
      move.b (SP)+,D0                     ;RESTORE D0
      rts

print_2hex::
      ror.b #4,D0
      bsr.s print_1hex
      rol.b #4,D0
      bra print_1hex

print_4hex::
      ror.w #8,D0
      bsr.s print_2hex
      rol.w #8,D0
      bra.s print_2hex

print_6hex::
      swap.w D0                           ; OUT 24-BIT HEX NUMBER
      bsr.s print_2hex
      swap.w D0
      bra.s print_4hex

print_8hex::
      swap.w D0                           ; OUT 32-BIT HEX NUMBER
      bsr.s  print_4hex
      swap.w D0
      bra.s  print_4hex
