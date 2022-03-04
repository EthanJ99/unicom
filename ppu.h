#pragma once

#include <stdint.h>

/*  PPU Memory Map:
    ---------------------------------------
    Address Range   Size    Description
    ---------------------------------------
    $0000-$0FFF 	$1000 	Pattern table 0
    $1000-$1FFF 	$1000 	Pattern table 1

    $2000-$23FF 	$0400 	Nametable 0 ====|
    $2400-$27FF 	$0400 	Nametable 1     | This 2kB area is sometimes referred to as the 'VRAM' of the system
    $2800-$2BFF 	$0400 	Nametable 2     |
    $2C00-$2FFF 	$0400 	Nametable 3 ====|

    $3000-$3EFF 	$0F00 	Mirrors of $2000-$2EFF
    $3F00-$3F1F 	$0020 	Palette RAM indexes
    $3F20-$3FFF 	$00E0 	Mirrors of $3F00-$3F1F
*/
typedef struct PPU{
    uint8_t memory[0x4000]; // 16kB memory
    uint8_t oam[256]; // 256 bytes Object Attribute Memory
} PPU;

// PPU register locations within CPU memory 0x2000 - 0x2007
enum {
    PPUCTRL = 0x2000,
    PPUMASK,
    PPUSTATUS,
    OAMADDR,
    OAMDATA,
    PPUSCROLL,
    PPUADDR,
    PPUDATA
};
// OAMDMA register 
# define OAMDMA 0x4014

void ppu_init(PPU* ppu);

void run_ppu(PPU* ppu);