#include "rom.h"

/*
typedef struct ROM{
    uint8_t prg_size;
} ROM;
*/

/*
    We load a ROM, assuming it is in iNES format. iNES is described here by the Nesdev Wiki (https://wiki.nesdev.org/w/index.php?title=INES):
    An iNES file consists of the following sections, in order:

        Header (16 bytes)
        Trainer, if present (0 or 512 bytes)
        PRG ROM data (16384 * x bytes)
        CHR ROM data, if present (8192 * y bytes)
        PlayChoice INST-ROM, if present (0 or 8192 bytes)
        PlayChoice PROM, if present (16 bytes Data, 16 bytes CounterOut) (this is often missing, see PC10 ROM-Images for details)

    Some ROM-Images additionally contain a 128-byte (or sometimes 127-byte) title at the end of the file. 
*/
int load_rom(char* path, CPU* cpu, PPU* ppu){
    FILE* rom = fopen(path, "rb");
    int prg_size;

    if(rom != NULL){

        // skip into header and read PRG-ROM size.
        fseek(rom, 4, SEEK_SET);
        fread(&prg_size, 1, 1, rom);

        // skip rest of header
        fseek(rom, 16, SEEK_SET);

        // read PRG ROM into cartridge memory space
        fread(&cpu->memory[0x8000], 1, 16384 * prg_size, rom);

        // Read CHR ROM into PPU memory
        // NROM has an 8KiB CHR ROM, which sits straight after the PRG ROM in iNES.
        // 16KiB == 16384 == 0x4000. We've already read 0x4000 + 16 == 0x4010 bytes (PRG ROM + 16 byte Header)
        fseek(rom, 0x4010, SEEK_SET);
        fread(ppu->memory, 1, 0x2000, rom);

        // mirror ROM if it won't fill memory
        if(prg_size < 2){
            fseek(rom, 16, SEEK_SET);
            fread(&cpu->memory[0xC000], 1, 16384 * prg_size, rom);
        }

        printf("PRG Size: %d\n", prg_size);
        fclose(rom);
        return 0;
    } 
    
    // ROM not found or other error
    return 1;
    
    
}