#pragma once

#include <stdint.h>
#include "cpu.h"
#include "ppu.h"

// A system containing all necessary components (CPU, PPU, etc)
typedef struct System{
    CPU* cpu;
    PPU* ppu;
} System;

// Informs compiler of global 'nes' variable; defined in main.c
extern System nes;

void system_init(CPU* cpu, PPU* ppu);
void system_tick();
uint8_t read(uint16_t addr);
void write(uint16_t addr, uint8_t data);

uint8_t cpu_read(uint16_t addr);
void cpu_write(uint16_t addr, uint8_t data);
void NMI();

uint8_t ppu_read(uint16_t addr);
void ppu_write(uint16_t addr, uint8_t data);