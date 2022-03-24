#pragma once

#include <stdint.h>
#include <SDL.h>
#include <stdbool.h>

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

#define PATTERN_TABLE_WIDTH  128
#define PATTERN_TABLE_HEIGHT 128
#define FRAME_WIDTH 256
#define FRAME_HEIGHT 240

typedef struct PPU{
    uint8_t memory[0x4000]; // 16kB memory
    uint8_t oam[256]; // 256 bytes Object Attribute Memory

    // Registers - any commented out are handled by other functions
    uint8_t reg_ppuctrl;
    uint8_t reg_ppumask;
    uint8_t reg_ppustatus;
    uint8_t reg_oamaddr;
    uint8_t reg_ppuscroll;
    uint8_t reg_oamdma;
    // uint8_t reg_oamdata;
    // uint8_t reg_ppuaddr;
    // uint8_t reg_ppudata;
    
    int ppu_cycles;
    int ppu_scanline;

    bool nmi_occurred;

    uint16_t ppu_addr;
    uint8_t ppu_read_buffer;

    // Intermediary storage (e.g. addr register buffer)
    uint8_t ppu_flag_update; // if 1, signals its time to write address to ppu_addr
    uint8_t ppu_latch;
} PPU;

// PPU register locations within CPU memory 0x2000 - 0x2007
typedef enum PPU_Reg {
    PPUCTRL = 0x2000,
    PPUMASK,
    PPUSTATUS,
    OAMADDR,
    OAMDATA,
    PPUSCROLL,
    PPUADDR,
    PPUDATA,
    OAMDMA = 0x4014
} PPU_Reg;

void ppu_init(PPU* ppu);
void ppu_step(PPU* ppu, SDL_Renderer* renderer);

uint8_t ppu_read_register(uint16_t addr);
uint8_t ppu_read_PPUSTATUS();
uint8_t ppu_read_OAMDATA();
uint8_t ppu_read_PPUDATA();

void ppu_write_register(uint16_t addr, uint8_t data);
void ppu_write_PPUCTRL(uint8_t data);
void ppu_write_PPUMASK(uint8_t data);
void ppu_write_OAMADDR(uint8_t data);
void ppu_write_OAMDATA(uint8_t data);
void ppu_write_PPUSCROLL(uint8_t data);
void ppu_write_PPUDATA(uint8_t data);
void ppu_write_PPUADDR(uint8_t data);
void ppu_write_OAMDMA(uint8_t data);



uint8_t ppu_get_vram_addr_increment();
uint8_t ppu_get_base_nametable_addr();
uint8_t ppu_get_nmi();
uint8_t ppu_get_vblank();
void ppu_set_vblank();
void ppu_clear_vblank();

void draw_pattern_table(SDL_Window* window, SDL_Renderer* renderer, uint16_t addr);
void draw_nametable(SDL_Renderer* renderer);
uint16_t get_pattern_table_address(PPU* ppu);

