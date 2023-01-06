;=====================================================================================================
; Ddraig v4 hardware addresses
;=====================================================================================================

SRAM_START      EQU     $00000000       ; Start of Static RAM
SRAM_END        EQU     $000FFFFF       ; End of Static RAM
DRAM_START      EQU     $00100000       ; Start of Dynamic RAM
DRAM_END        EQU     $008FFFFF       ; End of Dynamic RAM

DUART_BASE      EQU     $00F7F000       ; Base address of the Dual Asynchronous Receiver Transmitter
PIT_BASE        EQU     $00F7F100       ; Base address of the Peripheral Interface Timer
KBD_BASE        EQU     $00F7F200       ; Base address of the VT82C42 Keyboard/Mouse controller
IDE_BASE        EQU     $00F7F300       ; Base address of the IDE controller
RTC_BASE        EQU     $00F7F400       ; Base address of the RTC72421 clock controller
EXPID1_BASE     EQU     $00F7F500       ; Base address of Expansion ID 1
EXPID2_BASE     EQU     $00F7F600       ; Base address of Expansion ID 2
EXPID3_BASE     EQU     $00F7F700       ; Base address of Expansion ID 3
EXPID4_BASE     EQU     $00F7F800       ; Base address of Expansion ID 4

EXP1_BASE       EQU     $00A00000       ; Base address of Expansion Data 1
EXP2_BASE       EQU     $00B00000       ; Base address of Expansion Data 2
EXP3_BASE       EQU     $00C00000       ; Base address of Expansion Data 3
EXP4_BASE       EQU     $00D00000       ; Base address of Expansion Data 4


ROM_START       EQU     $00F80000       ; Start of ROM
ROM_END         EQU     $00FFFFFF       ; End of ROM
