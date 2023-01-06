;=====================================================================================================
; DUART Definitions file
;=====================================================================================================

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
