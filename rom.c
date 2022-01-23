#include "rom.h"

/*
typedef struct ROM{
    uint8_t prg_size;
} ROM;
*/

int load_rom(char* path, uint8_t* memory){
    FILE* rom = fopen(path, "rb");
    int prg_size;

    if(rom != NULL){

        // skip into header and read PRG-ROM size.
        fseek(rom, 4, SEEK_SET);
        fread(&prg_size, 1, 1, rom);

        // skip rest of header
        fseek(rom, 16, SEEK_SET);

        // read ROM into cartridge memory space
        fread(&memory[0x8000], 1, 16384 * prg_size, rom);

        // mirror ROM if it won't fill memory
        if(prg_size < 2){
            fseek(rom, 16, SEEK_SET);
            fread(&memory[0xC000], 1, 16384 * prg_size, rom);
        }

        fclose(rom);
        return 0;
    } 
    
    // ROM not found or other error
    return 1;
    
    
}