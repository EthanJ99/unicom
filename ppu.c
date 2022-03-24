#include <string.h>
#include <SDL.h>

#include "ppu.h"
#include "system.h"

uint8_t framebuffer_nt[256 * 960 * 3];	//	3 bytes per pixel, RGB24 - Nametables
uint8_t framebuffer[FRAME_WIDTH * FRAME_HEIGHT * 3];

void ppu_init(PPU* ppu){
    memset(ppu->memory, 0, sizeof(ppu->memory));
    memset(ppu->oam, 0, sizeof(ppu->oam));

    ppu->reg_ppuctrl = 0;
    ppu->reg_ppumask = 0;
    ppu->reg_ppustatus = 0;
    ppu->reg_oamaddr = 0;
    //ppu->reg_oamdata = 0;
    ppu->reg_ppuscroll = 0;
    //ppu->reg_ppuaddr = 0;
    //ppu->reg_ppudata = 0;
    ppu->reg_oamdma = 0;

    ppu->ppu_cycles = 0;
    ppu->ppu_scanline = 0;

    ppu->nmi_occurred = false;

    ppu->ppu_addr = 0;
    //ppu->ppu_vblank = 0;

    ppu->ppu_read_buffer = 0;

    ppu->ppu_flag_update = 0;
    ppu->ppu_latch = 0;
}

void ppu_step(PPU* ppu, SDL_Renderer* renderer){
    /* The following info borrowed (with love) from https://bugzmanov.github.io/nes_ebook/chapter_6_2.html 
    The NMI interrupt is tightly connected to PPU clock cycles:

    -the PPU renders 262 scan lines per frame
    -each scanline lasts for 341 PPU clock cycles
    -upon entering scanline 241, PPU triggers NMI interrupt
    -PPU clock cycles are 3 times faster than CPU clock cycles
    ========================================================
    PPU communicates that it's entering the VBLANK phase for the frame via another interrupt signal - NMI (Non-Maskable Interrupt). From a high-level perspective, this means two things:

    -PPU is done rendering the current frame
    -CPU can safely access PPU memory to update the state for the next frame.
    */

    ppu->ppu_cycles++;

    if(ppu->ppu_cycles >= 341){
        // move to next scanline after set num. cycles
        ppu->ppu_cycles -= 341;
        ppu->ppu_scanline++;
    }

    if(ppu->ppu_scanline >= 0 && ppu->ppu_scanline <= 239){
        // Visible scanlines (0 - 239)

    } else if(ppu->ppu_scanline == 241 && ppu->ppu_cycles == 1){
        // Enter vblank phase
        ppu_set_vblank();
        //printf("vblank... trying to trigger NMI...");
        // Signal CPU to perform an NMI when it can
        //if(ppu_get_nmi()){ // read from PPUCTRL
            printf("NMI NOW");
            ppu->nmi_occurred = true;
            nes.cpu->nmi = true;
        //}
        

    } else if(ppu->ppu_scanline >= 262 && ppu->ppu_cycles == 1){
        // Pre-render scanline (-1 or 261)
        ppu_clear_vblank();
        ppu->nmi_occurred = false;
        nes.cpu->nmi = false;
        ppu->ppu_scanline = 0;

    }
}


void draw_pattern_table(SDL_Window* window, SDL_Renderer* renderer, uint16_t addr){
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};

    SDL_Color* col[4] = {
        &black,
        &red,
        &green,
        &blue
    };

    // Loop through each tile (visualised as 256 tiles, in a 16x16 grid)
    for(int tile_y = 0; tile_y < 16; tile_y++){
        for(int tile_x = 0; tile_x < 16; tile_x++){
            
            // convert the tile x/y into a 1-dimensional index we can use to read from the PPU memory
            // add 256 rather than 16 because its 16 tiles of 16 bytes each on each row
            uint16_t pos = tile_y * 256 + tile_x * 16;

            // Loop through pixels in each tile (64 pixels, in a 8x8 grid)
            for(int pixel_y = 0; pixel_y < 8; pixel_y++){
                // separately fetch the low and high bytes of the current row from PPU memory.
                // the data is stored as all the low bytes of a tile (64 bits) followed by all the high bytes (another 64 bits)
                // addr will either be 0 or 0x1000 ('left' or 'right' pattern table). Offset by this amount when indexing the PPU memory.
                uint8_t row_low = ppu_read(addr + pos + pixel_y);
                uint8_t row_high = ppu_read(addr + pos + pixel_y + 8);

                for(int pixel_x = 0; pixel_x < 8; pixel_x++){
                    uint8_t pixel_val = (row_high & 1) << 1 | (row_low & 1);
                    // TODO fetch proper colour from palette table

                    row_high >>= 1;
                    row_low >>= 1;

                    // Actually draw the pixel now
                    SDL_SetRenderDrawColor(renderer, col[pixel_val]->r, col[pixel_val]->g, col[pixel_val]->b, col[pixel_val]->a);

                    // So far we've technically been reading the tile pixel data from right-to-left, so reverse that
                    uint8_t draw_x = tile_x * 8 + (7 - pixel_x);
                    uint8_t draw_y = tile_y * 8 + pixel_y;

                    if(addr == 0x1000){ // draw the table at 0x1000 on the right
                        SDL_RenderDrawPoint(renderer, draw_x + PATTERN_TABLE_WIDTH, draw_y);
                    } else{
                        SDL_RenderDrawPoint(renderer, draw_x, draw_y);
                    }
                }
            }


        }
    }
}

void draw_nametable(SDL_Renderer* renderer){
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color green = {0, 255, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};

    SDL_Color* colour[4] = {
        &black,
        &red,
        &green,
        &blue
    };

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, FRAME_WIDTH, FRAME_HEIGHT);

    // What is the current nametable?

    // What is the current position in the name table? (It is 30 rows, 32 tiles on each row)

        // What tile of the pattern table does this link to?

            // Within the tile, draw each pixel

    SDL_UpdateTexture(texture, NULL, framebuffer, 256 * sizeof(unsigned char) * 3);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}


uint16_t get_pattern_table_address(PPU* ppu){
    // read the pattern table address from PPUCTRL bit 4 (0 == 0x0000, 1 == 0x1000)
    return (ppu->reg_ppuctrl & 0x10) ? 0x1000 : 0; 
}

// ------------ DATA READ/WRITE ------------ //
uint8_t ppu_read_register(uint16_t addr){
    uint8_t data;
    switch(0x2000 + addr % 8){
        case PPUSTATUS: data = ppu_read_PPUSTATUS();  break;
        case OAMDATA:   data = ppu_read_OAMDATA();    break;
        case PPUDATA:   data = ppu_read_PPUDATA();    break;
        default:
            printf("Error! Trying to read from PPU reg. %.4X\n", addr);
            break;
    }
    return data;
}

uint8_t ppu_read_PPUSTATUS(){
    uint8_t data = nes.ppu->reg_ppustatus;

    // reading this register resets the latch
    nes.ppu->ppu_flag_update = 0;
    nes.ppu->ppu_latch = 0;

    // Also, clear the VBLANK flag (bit 7) in PPUSTATUS
    ppu_clear_vblank();
    return data;
}

uint8_t ppu_read_OAMDATA(){
    uint8_t data = nes.ppu->oam[nes.ppu->reg_oamaddr];
    return data;
    // account for reads during vblank/forced blank
}

uint8_t ppu_read_PPUDATA(){
    uint8_t data;
    if(nes.ppu->ppu_addr <=0x3eff){
        // emulate buffered read
        data = nes.ppu->ppu_read_buffer;
        nes.ppu->ppu_read_buffer = ppu_read(nes.ppu->ppu_addr);
    } else{
        // palette data isn't buffered
        data = nes.ppu->ppu_read_buffer;
    }
    return data;
}

void ppu_write_register(uint16_t addr, uint8_t data){
    switch(0x2000 + addr % 8){
        case PPUCTRL:   ppu_write_PPUCTRL(data);    break;
        case PPUMASK:   ppu_write_PPUMASK(data);    break;
        case OAMADDR:   ppu_write_OAMADDR(data);    break;
        case OAMDATA:   ppu_write_OAMDATA(data);    break;
        case PPUSCROLL: ppu_write_PPUSCROLL(data);  break;
        case PPUADDR:   ppu_write_PPUADDR(data);    break;
        case PPUDATA:   ppu_write_PPUDATA(data);    break;
        case OAMDMA:    ppu_write_OAMDMA(data);     break;
        default:
            printf("Error! Trying to write to PPU reg. %.4X\n", addr);
            break;
    }
}

void ppu_write_PPUCTRL(uint8_t data){
    nes.ppu->reg_ppuctrl = data;
}

void ppu_write_PPUMASK(uint8_t data){
    nes.ppu->reg_ppumask = data;
}

void ppu_write_OAMADDR(uint8_t data){
    nes.ppu->reg_oamaddr = data;
}

void ppu_write_OAMDATA(uint8_t data){
    //nes.ppu->reg_oamdata = data;
    nes.ppu->oam[nes.ppu->reg_oamaddr] = data;
    nes.ppu->reg_oamaddr++;
    

}

void ppu_write_PPUSCROLL(uint8_t data){
    nes.ppu->reg_ppuscroll = data;
}

void ppu_write_PPUADDR(uint8_t data){
    // 2xwrite
    // Load data into a buffer, then on 2nd write copy combined data to the actual register
    if(nes.ppu->ppu_flag_update == 1){
        nes.ppu->ppu_addr = (uint16_t)nes.ppu->ppu_latch | data;
        nes.ppu->ppu_latch = data; // write the new data to buf.; not really needed but oh well
        nes.ppu->ppu_flag_update = 0;
    } else{
        nes.ppu->ppu_latch = data;
        nes.ppu->ppu_flag_update = 1;
    }
}

void ppu_write_PPUDATA(uint8_t data){
    //nes.ppu->reg_ppudata = data;
    // write data to address stored in ppu.ppu_addr
    ppu_write(nes.ppu->ppu_addr, data);

    // increment address by 1 or 32
    nes.ppu->ppu_addr += ppu_get_vram_addr_increment();
}

void ppu_write_OAMDMA(uint8_t data){
    // fill the entire OAM with data from 0x??00 to 0x??FF in CPU memory
    uint16_t addr = (uint16_t)nes.ppu->reg_oamaddr << 8;

    for (int i = 0; i < 0x100; i++)
    {
        nes.ppu->oam[i] = read((addr | i));
    }
}

uint8_t ppu_get_base_nametable_addr(){
    uint8_t val = nes.ppu->reg_ppuctrl & 0x3;
    uint16_t addr;
    switch(val){
        case 0: addr = 0x2000; break;
        case 1: addr = 0x2400; break;
        case 2: addr = 0x2800; break;
        case 3: addr = 0x2C00; break;
    }
    return addr;
}

uint8_t ppu_get_vram_addr_increment(){
    // value in bit 2 of PPUCTRL reg determines value to increment vram
    // address (ppu.ppu_addr) by after write (0 == 1, 1 == 32);
    return (nes.ppu->reg_ppuctrl & 0x4) ? 32 : 1; 
}

uint8_t ppu_get_nmi(){
    // read bit 7 of PPUCTRL to determine if an NMI should be generated
    // at start of VBLANK
    uint8_t data = nes.ppu->reg_ppuctrl & 0x80;
    printf("NMI Status: %d\n", data);
    return (data); 
}

uint8_t ppu_get_vblank(){
    return nes.ppu->reg_ppustatus & 0x80;
}

void ppu_set_vblank(){
    nes.ppu->reg_ppustatus &= 0xFF; // 11111111
}

void ppu_clear_vblank(){
    nes.ppu->reg_ppustatus &= 0x7F; // 01111111
}