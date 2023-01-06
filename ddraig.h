#ifndef DDRAIG_OS_DDRAIG_H
#define DDRAIG_OS_DDRAIG_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "system/debug.h"

// Ddraig hardware addresses
#define SRAM_START      0x00000000       // Start of Static RAM
#define SRAM_END        0x000FFFFF       // End of Static RAM
#define DRAM_START      0x00100000       // Start of Dynamic RAM
#define DRAM_END        0x008FFFFF       // End of Dynamic RAM
#define HEAP_START      0x00020000       // Heap memory start
#define HEAP_END        0x008FFFFF       // Heap memory end

#define DUART_BASE      0x00F7F000       // Base address of the Dual Asynchronous Receiver Transmitter
#define PIT_BASE        0x00F7F100       // Base address of the Peripheral Interface Timer
#define PS2_BASE        0x00F7F200       // Base address of the VT82C42 Keyboard/Mouse controller
#define IDE_BASE        0x00F7F300       // Base address of the IDE controller
#define RTC_BASE        0x00F7F400       // Base address of the RTC72421 clock controller
#define EXP1_BASE       0x00F7F500       // Base address of the Expansion slot ID
#define EXP2_BASE       0x00F7F600       // Base address of the Expansion slot ID
#define EXP3_BASE       0x00F7F700       // Base address of the Expansion slot ID
#define EXP4_BASE       0x00F7F800       // Base address of the Expansion slot ID

#define EXP1_DATA       0x00A00000       // Base address of the Expansion slot data
#define EXP2_DATA       0x00B00000       // Base address of the Expansion slot data
#define EXP3_DATA       0x00C00000       // Base address of the Expansion slot data
#define EXP4_DATA       0x00D00000       // Base address of the Expansion slot data

#define ROM_START       0x00F80000       // Start of ROM
#define ROM_END         0x00FFFFFF       // End of ROM

#define STACK_SIZE      0x8000           // 32K for the stack

#define ISR_VECT_EXP1(vec)      (*((long *)0x64) = (long)vec)
#define ISR_VECT_IDE(vec)       (*((long *)0x68) = (long)vec)
#define ISR_VECT_PIT(vec)       (*((long *)0x6C) = (long)vec)
#define ISR_VECT_DUART(vec)     (*((long *)0x70) = (long)vec)
#define ISR_VECT_PS2(vec)       (*((long *)0x74) = (long)vec)
#define ISR_VECT_EXP2(vec)      (*((long *)0x78) = (long)vec)
#define ISR_VECT_EXP3(vec)      (*((long *)0x7C) = (long)vec)


#define EXPID_IDENTREG          0xFE
#define EXPID_REG(id, reg)      (*((volatile uint8_t *) (id + reg)))

#define DISABLE_INTERRUPTS()    __asm__ volatile("or.w	#0x0700, %sr");
#define ENABLE_INTERRUPTS()		__asm__ volatile("and.w	#0xF8FF, %sr");


#endif
