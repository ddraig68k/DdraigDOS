
	include 'ddraig.inc'
	include 'duart.inc'

; Use EASy68k Simulator
EASY68K_SIM	        EQU 0           ; Set 1 to use EASy68K Sim68K I/O
EASY68K_SERIAL	    EQU 0		    ; Use serial IO rather than keyboard

		ORG	$100000
start
	move.w	#$2700,SR	            ; diasble interrupts

	lea.l str_start,A3
	bsr print_string

	; Check the manufacturer ID
	lea.l str_manufacturer_id,A3
	bsr print_string
	jsr get_manufacturer_id
	jsr print_2hex
	jsr print_newline

	move.b #$BF,D1					; Check if the ID matches the expected value $BF
	cmp.b D1,D0
	beq device_id_ok
	lea.l str_manuf_mismatch,A3     ; Maufacturer ID mismatch, abort programming
	jsr print_string
	move.b D1,D0
	jsr print_2hex
	jsr print_newline
	jmp do_reboot

device_id_ok
	; Check the device ID
	lea.l str_device_id,A3
	bsr print_string
	jsr get_device_id
	jsr print_2hex
	jsr print_newline

	move.b #$B6,D1					; Check if the ID matches the expected value $B6
	cmp.b D1,D0
	beq start_progamming
	lea.l str_device_mismatch,A3    ; Device ID mismatch, abort programming
	jsr print_string
	move.b D1,D0
	jsr print_2hex
	jsr print_newline
	jmp do_reboot


start_progamming
	; A0 = Pointer to start of the new ROM data
	; A1 = Pointer to end of the new ROM data
	; A2 = ROM Start address
    move.l #flashdata_start,A0
    move.l #flashdata_end,A1
    move.l #ROM_START,A2

	jsr erase_chip
	jsr flash_program
	lea.l str_restart,A3
	bsr print_string
	jmp do_reboot

get_manufacturer_id
	; Enter Software ID mode
	move.w #$AAAA,ROM_START+($5555*2)
	move.w #$5555,ROM_START+($2AAA*2)
   	move.w #$9090,ROM_START+($5555*2)
   	move.w ROM_START+$0,D0
   	jsr exit_id_mode
   	rts

get_device_id
	; Enter Software ID mode
	move.w #$AAAA,ROM_START+($5555*2)
	move.w #$5555,ROM_START+($2AAA*2)
   	move.w #$9090,ROM_START+($5555*2)
   	move.w ROM_START+$002,D0
   	jsr exit_id_mode
   	rts

exit_id_mode
	; Exit Software ID mode
	move.w #$F0F0,ROM_START
	rts

erase_chip
	; Chipe erase command sequence
	lea.l str_device_erase,A3
	jsr print_string
	move.w #$AAAA,ROM_START+($5555*2)
	move.w #$5555,ROM_START+($2AAA*2)
   	move.w #$8080,ROM_START+($5555*2)
	move.w #$AAAA,ROM_START+($5555*2)
	move.w #$5555,ROM_START+($2AAA*2)
	move.w #$1010,ROM_START+($5555*2)
	move.l #ROM_START,A4
	bra wait_erase

wait_erase
	; Test Bit 7 and wait for it to go high.
	btst.b #7,(A4)
	beq wait_erase

	lea.l str_erase_done,A3
	jsr print_string
	rts

flash_program
	lea.l str_device_program,A3
	jsr print_string
	move.b #0,D2

flash_program_byte
	move.w #$AAAA,ROM_START+($5555*2)
	move.w #$5555,ROM_START+($2AAA*2)
   	move.w #$A0A0,ROM_START+($5555*2)
   	move.w (A0)+,(A2)+

wait_program
	; Read from Flash twice to toggle bit 6 status
	move.w ROM_START,D0
	cmp.w  ROM_START,D0
	bne wait_program

	addq.b #1,D2
	beq program_update

check_done
	; finished?
   	cmp.l  A0,A1
   	bne flash_program_byte
   	lea.l str_program_done,A3
	jsr print_string
	rts

program_update
    move.b #'*',D0
    bsr char_out
	bra check_done

do_reboot
    ; Small delay in case of duart still transmitting
    move.w #1000,D0
.reboot_del
    sub.w #1,D0
    bne .reboot_del

    move.l $F80004,A0
    jmp (A0)


    include 'functions.inc'

    align 2
* Variables and Strings

str_start           dc.b CR,LF,'Flash Updater utility.',CR,LF
str_start1          dc.b 'Checking for the 39SF020',CR,LF,$0
str_manufacturer_id dc.b 'Manufacturer ID    : ',$0
str_device_id       dc.b 'Device ID          : ',$0
str_manuf_mismatch  dc.b 'The Manufacturer ID does not match expected value, Update aborted',CR,LF,$0
str_device_mismatch dc.b 'The Device ID does not match expected value, Update aborted',CR,LF,$0
str_device_erase    dc.b 'Erasing Device     : ',$0
str_device_program  dc.b 'Programming Device : ',CR,LF,$0
str_erase_done      dc.b 'Erase finished',CR,LF,$0
str_program_done    dc.b CR,LF,'Programming finished',CR,LF,$0
str_restart         dc.b 'Restarting',CR,LF,CR,LF,$0

    align 2

flashdata_start
    incbin '../DdraigDOS.bin'

	; Align directive needed here before end. Odd number of bytes can cause
	; end address check during programming to fail
    align 2
flashdata_end

	END	START		; last line of source
