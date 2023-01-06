; Define the Ddraig address locations

SRAM_START      EQU     $00000000       ; Start of Static RAM
SRAM_END        EQU     $000FFFFF       ; End of Static RAM
DRAM_START      EQU     $00100000       ; Start of Dynamic RAM
DRAM_END        EQU     $008FFFFF       ; End of Dynamic RAM

KRAM_START      EQU     $00000400       ; Kernel RAM Start
KRAM_TOP        EQU     $00020000       ; Kernel RAM Top (128K)

HEAP_START      EQU     $00020000       ; Heap memory start
HEAP_END        EQU     $008FFFFF       ; Heap memory end

DUART_BASE      EQU     $00F7F000       ; Base address of the Dual Asynchronous Receiver Transmitter
PIT_BASE        EQU     $00F7F100       ; Base address of the Peripheral Interface Timer
KBD_BASE        EQU     $00F7F200       ; Base address of the VT82C42 Keyboard/Mouse controller
IDE_BASE        EQU     $00F7F300       ; Base address of the IDE controller
RTC_BASE        EQU     $00F7F400       ; Base address of the RTC72421 clock controller
EXPREG1_BASE    EQU     $00F7F500       ; Base address of Expansion ID 1
EXPREG2_BASE    EQU     $00F7F600       ; Base address of Expansion ID 2
EXPREG3_BASE    EQU     $00F7F700       ; Base address of Expansion ID 3
EXPREG4_BASE    EQU     $00F7F800       ; Base address of Expansion ID 4

EXPDATA1_BASE   EQU     $00A00000       ; Base address of Expansion Data 1
EXPDATA2_BASE   EQU     $00B00000       ; Base address of Expansion Data 2
EXPDATA3_BASE   EQU     $00C00000       ; Base address of Expansion Data 3
EXPDATA4_BASE   EQU     $00D00000       ; Base address of Expansion Data 4

ROM_START       EQU     $00F80000       ; Start of ROM
ROM_END         EQU     $00FFFFFF       ; End of ROM
