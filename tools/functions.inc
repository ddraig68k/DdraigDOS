; Useful collection of functions to speed up development

CR          EQU 13              ; ASCII Carriage return
LF          EQU 10              ; ASCII Line feed
SP          EQU 32              ; ASCII Space
BS          EQU 8               ; ASCII Backspace
RS          EQU $1E
ESC         EQU $1B
DEL	      EQU $7F		  ; Delete

; Print a NULL terminated string over serial
; Address of string to write in A2


; Create a small delay
; Delay value in D0
delay
    sub.l #1,D0
    cmp.l #0,D0
    bne delay
    rts


      IFEQ  EASY68K_SIM                   ; If not using the Easy68K simulator

init_duart
      move.l A0,-(SP)                     ; save registers
      lea DUART_BASE,A0
      move.b #$30,DUART_CRA(A0)           ; reset transmitter
      move.b #$20,DUART_CRA(A0)           ; reset receiver
      move.b #$10,DUART_CRA(A0)           ; reset MR pointer to MRx1

      move.b #$00,DUART_ACR(A0)           ; select BAUD rate set 1
      move.b #$CC,DUART_CSRA(A0)          ; set TX & RX to 38400 BAUD
      move.b #$93,DUART_MR1A(A0)          ; set 8bits,no parity,character mode,RxRDY IRQ,RxRTS enabled
      move.b #$27,DUART_MR2A(A0)          ; $37 Normal Mode,enable TxRTS & TxCTS,one Stop bit
      move.b #$05,DUART_CRA(A0)           ; enable Receiver & Transmitter
      move.b #$05,DUART_OPRSET(A0)        ; $01 assert rts
      move.l (SP)+,A0                     ; restore registers
      rts

char_out
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
char_ins
      movem.l A0/D1,-(SP)                 ; save registers
      lea DUART_BASE,A0
.cinspoll
      move.b DUART_SRA(A0),D1
      btst #RxRDY,D1
      beq .cinspoll
      move.b DUART_RBA(A0),D0
      movem.l (SP)+,A0/D1                 ; restore registers
      rts

char_in
      bsr char_ins
      bsr char_out
      rts

char_in_ready
      btst #RxRDY,(DUART_BASE+DUART_SRA)
      rts

	ENDC

  	IFNE	EASY68K_SIM

init_duart
      IFNE EASY68K_SERIAL
      ; initialize serial port
      move.l  #1<<16+40,d0  ; PID 1, task 40
      lea     com_port,A1       ; name of port
      trap   #15
      ;move.l  #1<<16+41,d0  ; PID 1, task 40
      ;move.l  #10,D1        ; 38400 Baud
      ;trap #15
      rts
      ENDC
      IFEQ EASY68K_SERIAL
      ; Keyboard Echo.
      ; D1.B = 0 to turn off keyboard echo.
      ; D1.B = non zero to enable it (default).
      ; Echo is restored on 'Reset' or when a new file is loaded.
      move.b #0,D1
      move.b #12,D0
      trap #15
      rts
      ENDC

char_out
      IFNE EASY68K_SERIAL
      movem.l D0-D1/A1,-(SP)
      lea ser_send_buf,A1	    ; Send buffer address
      move.b D0,(A1)
      move.l  #1<<16+43,d0  ; PID 1, task 40
      move.b #1,D1	    ; Send 1 character
      trap #15
      movem.l (SP)+,D0-D1/A1
      rts
      ENDC
      IFEQ EASY68K_SERIAL
      movem.l D0-D1,-(SP)  ; SAVE D1
      move.b D0,D1
      move.b #6,D0
      trap #15
      movem.l (SP)+,D0-D1 ; RESTORE D1
      rts
      ENDC

char_ins
      IFNE EASY68K_SERIAL
      movem.l D1/A1,-(SP)
      lea ser_rec_buf,A1	    ; Receive buffer address
      move.b D0,(A1)
.cins_wait
      move.l  #1<<16+42,d0  ; PID 1, task 40
      move.b #1,D1	    ; Get 1 character
      trap #15
      cmp.b #4,D0
      beq .cins_wait	    ; On timeout wait until get char
      lea ser_rec_buf,A1	    ; Receive buffer address
      move.b (A1),D0
      movem.l (SP)+,D1/A1
      rts
      ENDC
      IFEQ EASY68K_SERIAL
      move.l D1,-(SP)  ; SAVE D1
      move.b #5,D0
      trap #15
      move.b D1,D0  ;ACIAD.L,D0
      move.l (SP)+,D1 ; RESTORE D1
      rts
      ENDC

char_in
      bsr char_ins
      bsr char_out
      rts

char_in_ready
      move.l D1,-(SP)  ; SAVE D1
      move.b #7,D0
      trap #15
      move.b D1,D0  ;ACIAD.L,D0
      move.l (SP)+,D1 ; RESTORE D1
      btst #0,D0
      rts

      ENDC

; A3 point to start of string, terminated with 0
print_string::
      move.b (A3)+,D0
      cmp.b  #0,D0
      beq.s .pstring_end
      bsr char_out
      bra.s print_string
.pstring_end
      rts

; print a <cr><LF> combination for a new line
print_newline::
      move.l D0,-(SP)
      move.b #CR,D0
      bsr char_out
      move.b #LF,D0
      bsr char_out
      move.l (SP)+,D0
      rts

; NEW LINE
print_cr::
      move.l D0,-(SP)
      move.b #CR,D0
      bsr char_out
      move.l (SP)+,D0
      rts

; NEW LINE
print_lf::
      move.l D0,-(SP)
      move.b #LF,D0
      bsr char_out
      move.l (SP)+,D0
      rts

; PRINT HEX
; print_1hex = PRINT ONE HEX
; print_2hex = PRINT TWO
; print_4hex = PRINT FOUR
; print_8hex = PRINT EIGHT
; ENTRY: D0
print_1hex
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


;----------------------------------------------------------------------
; print_decimal
; D0 32-BIT BINARY NUMBER
print_decimal::
      movem.l A3-A6/D0,-(SP)
      adda.l #$400,A6          ; Buffer address
      bsr hex_to_decimal
      adda.l #$400,A3
      bsr print_string
      movem.l (SP)+,A3-A6/D0
      rts

hex_to_decimal::
      movem.l D1-D7,-(SP)                 ; SAVE REGISTERS
      move.l  D0,D7                       ; SAVE IT HERE
      bpl.s   HX2DC
      neg.l   D7                          ; CHANGE TO POSITIVE
      bmi.s   HX2DC57                     ; SPECIAL CASE (-0)
      move.b  #'-',(A6)+                  ; PUT IN NEG SIGN
HX2DC
      clr.w   D4                          ; FOR ZERO SURPRESS
      moveq.l   #10,D6                    ; COUNTER
HX2DC0
      moveq.l   #1,D2                           ; VALUE TO SUB
      move.l  D6,D1                       ; COUNTER
      subq.l  #1,D1                       ; ADJUST - FORM POWER OF TEN
      beq.s   HX2DC2                      ; IF POWER IS ZERO
HX2DC1
      move.w  D2,D3                       ; D3=LOWER WORD
      mulu.w    #10,D3
      swap.w    D2                        ; D2=UPPER WORD
      mulu.w    #10,D2
      swap.w    D3                        ; ADD UPPER TO UPPER
      add.w   D3,D2
      swap.w    D2                        ; PUT UPPER IN UPPER
      swap.w    D3                        ; PUT LOWER IN LOWER
      move.w  D3,D2                       ; D2=UPPER & LOWER
      subq.l  #1,D1
      bne     HX2DC1
HX2DC2
      clr.l   D0                          ; HOLDS SUB AMT
HX2DC22
      cmp.l   D2,D7
      blt.s   HX2DC3                      ; IF NO MORE SUB POSSIBLE
      addq.l  #1,D0                       ; BUMP SUBS
      sub.l   D2,D7                       ; COUNT DOWN BY POWERS OF TEN
      bra.s   HX2DC22                     ; DO MORE
HX2DC3
      tst.b   D0                          ; ANY VALUE?
      bne.s   HX2DC4
      tst.w  D4                           ; ZERO SURPRESS
      beq.s   HX2DC5
HX2DC4
      addi.b  #$30,D0                     ; BINARY TO ASCII
      move.b  D0,(A6)+                    ; PUT IN BUFFER
      move.b  D0,D4                       ; MARK AS NON ZERO SURPRESS
HX2DC5
      subq.l  #1,D6                       ; NEXT POWER
      bne     HX2DC0
      tst.w  D4                           ; SEE IF ANYTHING PRINTED
      bne.s   HX2DC6
HX2DC57
      move.b  #'0',(A6)+                  ; PRINT AT LEST A ZERO
HX2DC6
      move.b  #0,(A6)                     ; PUT TERMINATOR
      movem.l (SP)+,D1-D7                 ; RESTORE REGISTERS
      rts                                 ; END OF ROUTINE
