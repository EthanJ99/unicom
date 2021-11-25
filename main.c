#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "rom.h"
#include "dissassemble.h"
#include "ops.h"

int main(){
    CPU* cpu = cpu_init();

    // need to mirror this
    char* rom_path = "roms/dk.nes";
    load_rom(rom_path, cpu->memory);
    printf("Loaded ROM from '%s'\n", rom_path);

    // Set PC to first instruction
    cpu->pc =  cpu->memory[0xfffc] | (cpu->memory[0xfffd] << 8);
    printf("PC: %x (%d)\n", cpu->pc, cpu->pc);

    /* Main CPU execution loop
    while(cpu->pc <= 0xffff){

        uint8_t opcode = cpu->memory[cpu->pc++];
        switch(opcode){
            case 0xa9: LDA(cpu, addr_imm(cpu)); break;
            case 0xa5: LDA(cpu, addr_zpg(cpu)); break;
            case 0xb5: LDA(cpu, addr_abs(cpu)); break;
            case 0xad: LDA(cpu, addr_abx(cpu)); break;
            case 0xbd: LDA(cpu, addr_aby(cpu)); break;
            case 0xb9: LDA(cpu, addr_aby(cpu)); break;
            case 0xa1: LDA(cpu, addr_inx(cpu)); break;
            case 0xb1: LDA(cpu, addr_iny(cpu)); break;

            case 0xa2: LDX(cpu, addr_imm(cpu)); break;
            case 0xa6: LDX(cpu, addr_zpg(cpu)); break;
            case 0xb6: LDX(cpu, addr_zpy(cpu)); break;
            case 0xae: LDX(cpu, addr_abs(cpu)); break;
            case 0xbe: LDX(cpu, addr_aby(cpu)); break;
            
            case 0xa0: LDY(cpu, addr_imm(cpu)); break;
            case 0xa4: LDY(cpu, addr_zpg(cpu)); break;
            case 0xb4: LDY(cpu, addr_zpx(cpu)); break;
            case 0xac: LDY(cpu, addr_abs(cpu)); break;
            case 0xbc: LDY(cpu, addr_abx(cpu)); break;

            case 0x85: STA(cpu, addr_zpg(cpu)); break;
            case 0x95: STA(cpu, addr_zpx(cpu)); break;
            case 0x8d: STA(cpu, addr_abs(cpu)); break;
            case 0x9d: STA(cpu, addr_abx(cpu)); break;
            case 0x99: STA(cpu, addr_aby(cpu)); break;
            case 0x81: STA(cpu, addr_inx(cpu)); break;
            case 0x91: STA(cpu, addr_iny(cpu)); break;

            case 0x86: STX(cpu, addr_zpg(cpu)); break;
            case 0x96: STX(cpu, addr_zpy(cpu)); break;
            case 0x8e: STX(cpu, addr_abs(cpu)); break;

            case 0x84: STY(cpu, addr_zpg(cpu)); break;
            case 0x94: STY(cpu, addr_zpy(cpu)); break;
            case 0x8c: STY(cpu, addr_abs(cpu)); break;

            case 0xaa: TAX(cpu);                break;
            case 0xa8: TAY(cpu);                break;
            case 0x8a: TXA(cpu);                break;
            case 0x98: TYA(cpu);                break;

            case 0xba: TSX(cpu);                break;
            case 0x9a: TXS(cpu);                break;
            case 0x48: PHA(cpu);                break;
            case 0x08: PHP(cpu);                break;
            case 0x68: PLA(cpu);                break;
            case 0x28: PLP(cpu);                break;

            case 0x29: AND(cpu, addr_imm(cpu)); break;
            case 0x25: AND(cpu, addr_zpg(cpu)); break;
            case 0x35: AND(cpu, addr_zpx(cpu)); break;
            case 0x2d: AND(cpu, addr_abs(cpu)); break;
            case 0x3d: AND(cpu, addr_abx(cpu)); break;
            case 0x39: AND(cpu, addr_aby(cpu)); break;
            case 0x21: AND(cpu, addr_inx(cpu)); break;
            case 0x31: AND(cpu, addr_iny(cpu)); break;

            case 0x49: EOR(cpu, addr_imm(cpu)); break;
            case 0x45: EOR(cpu, addr_zpg(cpu)); break;
            case 0x55: EOR(cpu, addr_zpx(cpu)); break;
            case 0x4d: EOR(cpu, addr_abs(cpu)); break;
            case 0x5d: EOR(cpu, addr_abx(cpu)); break;
            case 0x59: EOR(cpu, addr_aby(cpu)); break;
            case 0x41: EOR(cpu, addr_inx(cpu)); break;
            case 0x51: EOR(cpu, addr_iny(cpu)); break;

            case 0x09: ORA(cpu, addr_imm(cpu)); break;
            case 0x05: ORA(cpu, addr_zpg(cpu)); break;
            case 0x15: ORA(cpu, addr_zpx(cpu)); break;
            case 0x0d: ORA(cpu, addr_abs(cpu)); break;
            case 0x1d: ORA(cpu, addr_abx(cpu)); break;
            case 0x19: ORA(cpu, addr_aby(cpu)); break;
            case 0x01: ORA(cpu, addr_inx(cpu)); break;
            case 0x11: ORA(cpu, addr_iny(cpu)); break;

            //ADC

            //SBC

            case 0xc9: CMP(cpu, addr_imm(cpu)); break;
            case 0xc5: CMP(cpu, addr_zpg(cpu)); break;
            case 0xd5: CMP(cpu, addr_zpx(cpu)); break;
            case 0xcd: CMP(cpu, addr_abs(cpu)); break;
            case 0xdd: CMP(cpu, addr_abx(cpu)); break;
            case 0xd9: CMP(cpu, addr_aby(cpu)); break;
            case 0xc1: CMP(cpu, addr_inx(cpu)); break;
            case 0xd1: CMP(cpu, addr_iny(cpu)); break;

            case 0xe0: CPX(cpu, addr_imm(cpu)); break;
            case 0xe4: CPX(cpu, addr_zpg(cpu)); break;
            case 0xec: CPX(cpu, addr_abs(cpu)); break;

            case 0xc0: CPX(cpu, addr_imm(cpu)); break;
            case 0xc4: CPX(cpu, addr_zpg(cpu)); break;
            case 0xcc: CPX(cpu, addr_abs(cpu)); break;

        }
    } 

    */

    // Run Dissassembler
    print_dissassembly(cpu);


    return 0;
}