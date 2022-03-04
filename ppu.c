#include <string.h>

#include "ppu.h"

void ppu_init(PPU* ppu){
    memset(ppu->memory, 0, sizeof(ppu->memory));
    memset(ppu->oam, 0, sizeof(ppu->oam));
}