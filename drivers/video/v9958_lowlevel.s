    section .text

DM_PAL            equ   2
DM_NTSC           equ   0

DISPMODE          equ   DM_PAL            ; DM_PAL or DM_NTSC

BACKGROUND1       equ   %00000000         ; BG: 0 R2 R1 R0 0 B2 B1 B0
BACKGROUND2       equ   %00000000         ; BG: 0 0  0  0  0 G2 G1 G0
FOREGROUND1       equ   %01110111         ; FG: 0 R2 R1 R0 0 B2 B1 B0
FOREGROUND2       equ   %00000111         ; FG: 0 0  0  0  0 G2 G1 G0

PORT_RWDATA       equ   $0
PORT_WREG_SETUP   equ   $2
PORT_WPALETTE     equ   $4
PORT_WREG_DATA    equ   $6

LINELENGTH        equ   80                ; Must be a multiple of 4!
DWLINELENGTH      equ   LINELENGTH/4
LINECOUNT         equ   24
DISPLAYSIZE       equ   LINELENGTH*LINECOUNT

; Constants for addressing video RAM
VDP_REG2_PAGE0    equ   $3           ; Page 0 at 0x0
VDP_REG2_PAGE1    equ   $B           ; Page 1 at 0x2000

VRAM_HIGH         equ   $0           ; VRAM A14-16 for any page
VRAM_LOW          equ   $0           ; VRAM A0-SP for any page (base)

VRAM_MID_PAGE0_R  equ   $0           ; VRAM A8-A13 for page 0 (Read)
VRAM_MID_PAGE0_W  equ   $40          ; VRAM A8-A13 for page 0 (Write)
VRAM_MID_PAGE1_R  equ   $20          ; VRAM A8-A13 for page 1 (Read)
VRAM_MID_PAGE1_W  equ   $60          ; VRAM A8-A13 for page 1 (Write)


; v9958_ll_init_textmode
; A0 - pointer to v9958 base address
v9958_ll_init_textmode::
    movem.l d0-d2/a1,-(sp)

    move.w  SR,D2                             ; Store SR
    ori.w   #$0200,SR                         ; No interrupts during init...

    move.b  #0,CURRENTPAGE

    ; Setup Palette
    move.b  #%00000000,(PORT_WREG_SETUP,A0)   ; Start at palette register 0
    move.b  #$90,(PORT_WREG_SETUP,A0)         ; .. in VDP register 16
    move.b  #BACKGROUND1,(PORT_WPALETTE,A0)
    move.b  #BACKGROUND2,(PORT_WPALETTE,A0)
    move.b  #FOREGROUND1,(PORT_WPALETTE,A0)
    move.b  #FOREGROUND2,(PORT_WPALETTE,A0)

    move.b  #%00010000,(PORT_WREG_SETUP,A0)   ; Select FG1, BG 0
    move.b  #$87,(PORT_WREG_SETUP,A0)         ; .. In VDP register 7

    move.b  #%00001010,(PORT_WREG_SETUP,A0)   ; Select 64K DRAM, disable sprites
    move.b  #$88,(PORT_WREG_SETUP,A0)         ; .. In VDP register 8

    ; Set pattern generator table to 0x1000
    move.b  #2,(PORT_WREG_SETUP,A0)           ; bits 16-11 of 0x1000
    move.b  #$84,(PORT_WREG_SETUP,A0)         ; .. to register 4

    ; Set pattern layout table to page 0
    move.b  #VDP_REG2_PAGE0,(PORT_WREG_SETUP,A0)
    move.b  #$82,(PORT_WREG_SETUP,A0)         ; .. to register 2

    ; Set PAL/NTSC mode (TODO should be runtime configurable...)
    move.b  #DISPMODE,(PORT_WREG_SETUP,A0)    ; Write PAL/NTSC mode
    move.b  #$89,(PORT_WREG_SETUP,A0)         ; .. to register 9

    ; Set up to write VRAM at 0x1000
    ; 0x1000 equates to 00001000000000000, which is split across the writes as:
    ; Write 1: bits 16-14 = (00000)000  = 0
    ; Write 2: bits 7-0   = 00000000    = 0
    ; Write 3: bits 13-8  = (01)010000  = $50 ($40 is write-enable)
    move.b  #0,(PORT_WREG_SETUP,A0)           ; VRAM Base at 0
    move.b  #$8E,(PORT_WREG_SETUP,A0)         ; > register 14
    move.b  #0,(PORT_WREG_SETUP,A0)           ; Set VRAM A0-SP
    move.b  #$50,(PORT_WREG_SETUP,A0)         ; Set VRAM A8-A13, and write enable

    ; Load font to pattern table
    lea     _V9958_font_begin,A1
    move.l  #_V9958_font_end,D0
    sub.l   A1,D0
    bra.s   .PATTERN_FILL

.PATTERN_FILL_LOOP
    move.b  (A1)+,(A0)                        ; Copy data
    nop                                       ; Small slow-down needed here..
    nop
    nop
    nop
    nop
    nop
    nop
.PATTERN_FILL:
    dbra.w  D0,.PATTERN_FILL_LOOP

    ; Clear the main memory buffer and copy to the VRAM
    jsr   int_v9958_clear_display

    ; Set text mode 1, enable interrupts and turn display on
    move.b  #%00000100,(PORT_WREG_SETUP,A0)   ; Write DG=0,IE2=0,IE1=0,M5=0,M4=0,M3=0
    move.b  #$80,(PORT_WREG_SETUP,A0)         ; To register 0

    move.b  #%01010000,(PORT_WREG_SETUP,A0)   ; Write BL=1,IE0=0,M1=1,M2=0,SI=0,MAG=0
    move.b  #$81,(PORT_WREG_SETUP,A0)         ; To register 1

    move.w  D2,SR                             ; Restore SR

    movem.l (SP)+,D0-D2/A1
    rts

; v9958_ll_set_address
; A0 - pointer to v9958 base address
; D0 - Address to set
v9958_ll_set_write_address::
    move.l  D1,-(SP)
    move.l  D2,-(SP)
    move.l  D0,D1           ; Store address

    ; Set up to write VRAM at 0x0
    move.b  #14,D2
    lsr.l   D2,D0
    and.b   #7,D0
    move.b  D0,(PORT_WREG_SETUP,A0)           ; VRAM high address
    nop
    move.b  #$8E,(PORT_WREG_SETUP,A0)         ; > register 14
    move.l  D1,D0
    move.b  D0,(PORT_WREG_SETUP,A0)           ; VRAM low byte

    move.l  D1,D0
    move.b  #8,D2
    lsr.l   D2,D0
    and.b   #$3f,D0
    or.b    #$40,D0
    move.b  D0,(PORT_WREG_SETUP,A0)          ; VRAM A8-A13 and write enable

    move.l  (SP)+,D2
    move.l  (SP)+,D1
    rts


; v9958_ll_write_data
; A0 - pointer to v9958 base address
; A1 - pointer to buffer address
; D0 - bytes to write
v9958_ll_write_data::
    move.l D2,-(SP)

    move.w  SR,D2                             ; Disable ints when copying

    bra.s   .PATTERN_FILL
.PATTERN_FILL_LOOP
    move.b  (A1)+,(A0)                        ; Copy data
    nop                                       ; Small slow-down needed here..
    nop
    nop
    nop
    nop
    nop
    nop
.PATTERN_FILL:
    dbra.w  D0,.PATTERN_FILL_LOOP

    move.w  D2,SR                             ; Restore SR

    move.l  (SP)+,D2
    rts


; v9958_ll_clear_display()
; A0 - pointer to v9958 base address
v9958_ll_clear_display::
    jsr int_v9958_clear_display
    rts

v9958_ll_copy_buffer:

v9958_ll_write_text:

v9958_ll_write_char:

; Internal functions
; Clear the display for the current page
int_v9958_clear_display:
    move.w  D2,-(SP)
    move.l  D1,-(SP)
    move.w  SR,D2                             ; Store SR
    ori.w   #$0200,SR                         ; Cannot be interrupted for a bit...

    ; Set up to write VRAM at 0x0
    move.b  #VRAM_HIGH,(PORT_WREG_SETUP,A0)   ; VRAM Base at 0
    move.b  #$8E,(PORT_WREG_SETUP,A0)         ; > register 14
    move.b  #VRAM_LOW,(PORT_WREG_SETUP,A0)    ; Set VRAM A0-SP

    ; Set CURRENTPAGE to the destination page
    move.b  D0,CURRENTPAGE
    beq.s   .PAGEZERO

    ; Page 1
    move.b  #VRAM_MID_PAGE1_W,(PORT_WREG_SETUP,A0)
    bra.s   .GOGOGO

.PAGEZERO:
    move.b  #VRAM_MID_PAGE0_W,(PORT_WREG_SETUP,A0)

.GOGOGO
    move.w  #0,D0
    move.w  #DISPLAYSIZE,D1

    bra.s   .COPY
.COPYLOOP
    move.b  #0,(A0)                      ; Must slow down here.
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    addq.w  #1,D0
    cmpi.w  #DISPLAYSIZE,D0
    bne.s   .COPY

    moveq.l #0,D0

.COPY
    dbra.w  D1,.COPYLOOP

    move.w  D2,SR                             ; Go for interrupts again...
    move.l  (SP)+,D1                          ; ... and we're done.
    move.w  (SP)+,D2
    rts


    section .data
CURRENTPAGE   dc.b      0
