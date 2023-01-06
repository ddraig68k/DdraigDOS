

isr_bus_error::
 ;   lea.l   bus_error_str,A1
 ;   jsr     print_string
    rte

isr_address_error::
 ;   lea.l   addr_error_str,A1
;    jsr     print_string
    rte

isr_illegal_instruction::
;    lea.l   inst_error_str,A1
;    jsr     print_string
    rte

isr_generic::
    rte

    section .rodata
bus_error_str       dc.b 13,10,'Bus exception error',13,10,0
addr_error_str      dc.b 13,10,'Address exception error',13,10,0
inst_error_str      dc.b 13,10,'Illegal instruction error',13,10,0
