#include "system.h"
#include "cpu.h"
#include "ppu.h"

void system_init(CPU* cpu, PPU* ppu){
    nes.cpu = cpu;
    nes.ppu = ppu;
}

void system_tick(){

}

uint8_t read(uint16_t addr){
    uint8_t data = 0;

    if(addr >= 0x0000 && addr <= 0x1FFF){
        // Main memory
        // Values are read/written up to 0x07FF, then mirrored through to 0x1FFF
        data = nes.cpu->memory[addr % 0x0800];

    } else if(addr >= 0x2000 && addr <= 0x3FFF){
        // PPU registers
        // Values are read/written up to 0x2007, then mirrored through to 0x3FFF (every 8 bytes)

        data = ppu_read_register(addr);

    } else if(addr >= 0x4020 && addr <= 0xFFFF){
        // Cartridge space
        // This is wrong; CPU currently contains entire addressable memory range (should only have 2KiB). For now read from it anyway:
        data = nes.cpu->memory[addr];

    } else{
        // Invalid address
        printf("[Error] Trying to read from invalid address %.4x.\n", addr);
    }

    return data;
}

void write(uint16_t addr, uint8_t data){
    if(addr >= 0x0000 && addr <= 0x1FFF){
        // Main memory
        // Values are read/written up to 0x07FF, then mirrored through to 0x1FFF
        nes.cpu->memory[addr % 0x0800] = data;

    } else if(addr >= 0x2000 && addr <= 0x3FFF){
        // PPU registers
        // Values are read/written up to 0x2007, then mirrored through to 0x3FFF (every 8 bytes)

        ppu_write_register(addr, data);

    } else if(addr == 0x4014){
        // PPU OAM DMA register
        ppu_write_register(addr, data);
    } else if(addr >= 0x4020 && addr <= 0xFFFF){
        // Cartridge space
        // This is wrong; CPU currently contains entire addressable memory range (should only have 2KiB). For now write to it anyway:
        nes.cpu->memory[addr] = data;

    } else{
        // Invalid address
        printf("[Error] Trying to write to invalid address %.4x.\n", addr);
    }

}

uint8_t ppu_read(uint16_t addr){
    return nes.ppu->memory[addr];
}

void ppu_write(uint16_t addr, uint8_t data){
    
}