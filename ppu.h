#include <stdint.h>

typedef struct PPU{
    uint8_t memory[0x4000]; // 16kB memory
    uint8_t oam[256]; // 256 bytes Object Attribute Memory
};

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

void init_ppu(PPU* ppu);

void run_ppu(PPU* ppu);