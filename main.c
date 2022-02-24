#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "cpu.h"
#include "rom.h"
#include "ops.h"

int main(){
    CPU* cpu = cpu_init();

    // need to mirror this
    /*char* rom_path = "roms/dk.nes";
    int rom_status = load_rom(rom_path, cpu->memory);*/

    // Load the NESTEST rom
    char* rom_path = "roms/nestest.nes";
    int rom_status = load_rom(rom_path, cpu->memory);

    if(rom_status == 0){
        printf("Loaded ROM from '%s'\n", rom_path);
    } else{
        printf("ERROR! Failed to load ROM from '%s'\n", rom_path);
        exit(1);
    }

    // Set PC to first instruction
    //cpu->pc =  cpu->memory[0xfffc] | (cpu->memory[0xfffd] << 8);

    // Set PC to NESTEST start point
    cpu->pc = 0xc000;
    printf("PC: %x (%d)\n", cpu->pc, cpu->pc);

    bool running = true;

    int loop = 1;

    // Main execution loop
    while(running){

        // Get basic data about current opcode
        uint8_t opcode = cpu->memory[cpu->pc];
        
        Op* op = get_op_data(opcode);
        Mode addr_mode = op->mode;

        // print debug information
        printf("[%d] ", loop);
        disassemble_op(cpu, op);
        printf("\tA:%.2X X:%.2X Y:%.2X P:%.2X SP:%.2X ", cpu->a, cpu->x, cpu->y, cpu->p, cpu->s);
        
        cpu->pc++;
        
        switch(opcode){
            case 0x00: BRK(cpu); break;
            case 0x01: ORA(cpu, addr_inx(cpu)); break;
            case 0x05: ORA(cpu, addr_zpg(cpu)); break;
            case 0x06: ASL(cpu, addr_zpg(cpu), addr_mode); break;
            case 0x08: PHP(cpu); break;
            case 0x09: ORA(cpu, addr_imm(cpu)); break;
            case 0x0a: ASL(cpu, addr_acc(cpu), addr_mode); break;
            case 0x0d: ORA(cpu, addr_abs(cpu)); break;
            case 0x0e: ASL(cpu, addr_abs(cpu), addr_mode); break;
            case 0x10: BPL(cpu, addr_rel(cpu)); break;
            case 0x11: ORA(cpu, addr_iny(cpu)); break;
            case 0x15: ORA(cpu, addr_zpx(cpu)); break;
            case 0x16: ASL(cpu, addr_zpx(cpu), addr_mode); break;
            case 0x18: CLC(cpu); break;
            case 0x19: ORA(cpu, addr_aby(cpu)); break;
            case 0x1d: ORA(cpu, addr_abx(cpu)); break;
            case 0x1e: ASL(cpu, addr_abx(cpu), addr_mode); break;
            case 0x20: JSR(cpu, addr_abs(cpu)); break;
            case 0x21: AND(cpu, addr_inx(cpu)); break;
            case 0x24: BIT(cpu, addr_zpg(cpu)); break;
            case 0x25: AND(cpu, addr_zpg(cpu)); break;
            case 0x26: ROL(cpu, addr_zpg(cpu), addr_mode); break;
            case 0x28: PLP(cpu); break;
            case 0x29: AND(cpu, addr_imm(cpu)); break;
            case 0x2a: ROL(cpu, addr_acc(cpu), addr_mode); break;
            case 0x2c: BIT(cpu, addr_abs(cpu)); break;
            case 0x2d: AND(cpu, addr_abs(cpu)); break;
            case 0x2e: ROL(cpu, addr_abs(cpu), addr_mode); break;
            case 0x30: BMI(cpu, addr_rel(cpu)); break;
            case 0x31: AND(cpu, addr_iny(cpu)); break;
            case 0x35: AND(cpu, addr_zpx(cpu)); break;
            case 0x36: ROL(cpu, addr_zpx(cpu), addr_mode); break;
            case 0x38: SEC(cpu); break;
            case 0x39: AND(cpu, addr_aby(cpu)); break;
            case 0x3d: AND(cpu, addr_abx(cpu)); break;
            case 0x3e: ROL(cpu, addr_abx(cpu), addr_mode); break;
            case 0x40: RTI(cpu); break;
            case 0x41: EOR(cpu, addr_inx(cpu)); break;
            case 0x45: EOR(cpu, addr_zpg(cpu)); break;
            case 0x46: LSR(cpu, addr_zpg(cpu), addr_mode); break;
            case 0x48: PHA(cpu); break;
            case 0x49: EOR(cpu, addr_imm(cpu)); break;
            case 0x4a: LSR(cpu, addr_acc(cpu), addr_mode); break;
            case 0x4c: JMP(cpu, addr_abs(cpu)); break;
            case 0x4d: EOR(cpu, addr_abs(cpu)); break;
            case 0x4e: LSR(cpu, addr_abs(cpu), addr_mode); break;
            case 0x50: BVC(cpu, addr_rel(cpu)); break;
            case 0x51: EOR(cpu, addr_iny(cpu)); break;
            case 0x55: EOR(cpu, addr_zpx(cpu)); break;
            case 0x56: LSR(cpu, addr_zpx(cpu), addr_mode); break;
            case 0x58: CLI(cpu); break;
            case 0x59: EOR(cpu, addr_aby(cpu)); break;
            case 0x5D: EOR(cpu, addr_abx(cpu)); break;
            case 0x5E: LSR(cpu, addr_abx(cpu), addr_mode); break;
            case 0x60: RTS(cpu); break;
            case 0x61: ADC(cpu, addr_inx(cpu)); break;
            case 0x65: ADC(cpu, addr_zpg(cpu)); break;
            case 0x66: ROR(cpu, addr_zpg(cpu), addr_mode); break;
            case 0x68: PLA(cpu); break;
            case 0x69: ADC(cpu, addr_imm(cpu)); break;
            case 0x6a: ROR(cpu, addr_acc(cpu), addr_mode); break;
            case 0x6c: JMP(cpu, addr_ind(cpu)); break;
            case 0x6d: ADC(cpu, addr_abs(cpu)); break;
            case 0x6e: ROR(cpu, addr_abs(cpu), addr_mode); break;
            case 0x70: BVS(cpu, addr_rel(cpu)); break;
            case 0x71: ADC(cpu, addr_iny(cpu)); break;
            case 0x75: ADC(cpu, addr_zpx(cpu)); break;
            case 0x76: ROR(cpu, addr_zpx(cpu), addr_mode); break;
            case 0x78: SEI(cpu); break;
            case 0x79: ADC(cpu, addr_abs(cpu)); break;
            case 0x7d: ADC(cpu, addr_abx(cpu)); break;
            case 0x7e: ROR(cpu, addr_abx(cpu), addr_mode); break;
            case 0x81: STA(cpu, addr_inx(cpu)); break;
            case 0x84: STY(cpu, addr_zpg(cpu)); break;
            case 0x85: STA(cpu, addr_zpg(cpu)); break;
            case 0x86: STX(cpu, addr_zpg(cpu)); break;
            case 0x88: DEY(cpu); break;
            case 0x8a: TXA(cpu); break;
            case 0x8c: STY(cpu, addr_abs(cpu)); break;
            case 0x8d: STA(cpu, addr_abs(cpu)); break;
            case 0x8e: STX(cpu, addr_abs(cpu)); break;    
            case 0x90: BCC(cpu, addr_rel(cpu)); break;
            case 0x91: STA(cpu, addr_iny(cpu)); break;
            case 0x94: STY(cpu, addr_zpx(cpu)); break;
            case 0x95: STA(cpu, addr_zpx(cpu)); break;
            case 0x96: STX(cpu, addr_zpy(cpu)); break;
            case 0x98: TYA(cpu); break;
            case 0x99: STA(cpu, addr_aby(cpu)); break;
            case 0x9a: TXS(cpu); break;
            case 0x9d: STA(cpu, addr_abx(cpu)); break;
            case 0xa0: LDY(cpu, addr_imm(cpu)); break;
            case 0xa1: LDA(cpu, addr_inx(cpu)); break;
            case 0xa2: LDX(cpu, addr_imm(cpu)); break;
            case 0xa4: LDY(cpu, addr_zpg(cpu)); break;
            case 0xa5: LDA(cpu, addr_zpg(cpu)); break;
            case 0xa6: LDX(cpu, addr_zpg(cpu)); break;
            case 0xa8: TAY(cpu); break;
            case 0xa9: LDA(cpu, addr_imm(cpu)); break;
            case 0xaa: TAX(cpu); break;
            case 0xac: LDY(cpu, addr_abs(cpu)); break;
            case 0xad: LDA(cpu, addr_abs(cpu)); break;
            case 0xae: LDX(cpu, addr_abs(cpu)); break;    
            case 0xb0: BCS(cpu, addr_rel(cpu)); break;
            case 0xb1: LDA(cpu, addr_iny(cpu)); break;
            case 0xb4: LDY(cpu, addr_zpx(cpu)); break;
            case 0xb5: LDA(cpu, addr_zpx(cpu)); break;
            case 0xb6: LDX(cpu, addr_zpy(cpu)); break;
            case 0xb8: CLV(cpu); break;
            case 0xb9: LDA(cpu, addr_aby(cpu)); break;
            case 0xba: TSX(cpu); break;
            case 0xbc: LDY(cpu, addr_abx(cpu)); break;
            case 0xbd: LDA(cpu, addr_abx(cpu)); break;
            case 0xbe: LDX(cpu, addr_aby(cpu)); break;
            case 0xc0: CPY(cpu, addr_imm(cpu)); break;
            case 0xc1: CMP(cpu, addr_inx(cpu)); break;
            case 0xc4: CPY(cpu, addr_zpg(cpu)); break;
            case 0xc5: CMP(cpu, addr_zpg(cpu)); break;
            case 0xc6: DEC(cpu, addr_zpg(cpu)); break;
            case 0xc8: INY(cpu); break;
            case 0xc9: CMP(cpu, addr_imm(cpu)); break;
            case 0xca: DEX(cpu); break;
            case 0xcc: CPY(cpu, addr_abs(cpu)); break;
            case 0xcd: CMP(cpu, addr_abs(cpu)); break;
            case 0xce: DEC(cpu, addr_abs(cpu)); break;
            case 0xd0: BNE(cpu, addr_rel(cpu)); break;
            case 0xd1: CMP(cpu, addr_iny(cpu)); break;
            case 0xd5: CMP(cpu, addr_zpx(cpu)); break;
            case 0xd6: DEC(cpu, addr_zpx(cpu)); break;
            case 0xd8: CLD(cpu); break;
            case 0xd9: CMP(cpu, addr_aby(cpu)); break;
            case 0xdd: CMP(cpu, addr_abx(cpu)); break;
            case 0xde: DEC(cpu, addr_abx(cpu)); break;
            case 0xe0: CPX(cpu, addr_imm(cpu)); break;
            case 0xe1: SBC(cpu, addr_inx(cpu)); break;
            case 0xe4: CPX(cpu, addr_zpg(cpu)); break;
            case 0xe5: SBC(cpu, addr_zpg(cpu)); break;
            case 0xe6: INC(cpu, addr_zpg(cpu)); break;
            case 0xe8: INX(cpu); break;
            case 0xe9: SBC(cpu, addr_imm(cpu)); break;
            case 0xea: NOP(cpu); break;
            case 0xec: CPX(cpu, addr_abs(cpu)); break;
            case 0xed: SBC(cpu, addr_abs(cpu)); break;
            case 0xee: INC(cpu, addr_abs(cpu)); break;
            case 0xf0: BEQ(cpu, addr_rel(cpu)); break;
            case 0xf1: SBC(cpu, addr_iny(cpu)); break;
            case 0xf5: SBC(cpu, addr_zpx(cpu)); break;
            case 0xf6: INC(cpu, addr_zpx(cpu)); break;
            case 0xf8: SED(cpu); break;
            case 0xf9: SBC(cpu, addr_aby(cpu)); break;
            case 0xfd: SBC(cpu, addr_abx(cpu)); break;
            case 0xfe: INC(cpu, addr_abx(cpu)); break;

            // Currently handles invalid ops by just using the NOP instruction
            default:
                NOP(cpu);
                break;
        }

        printf("\n");
        fflush(stdout);
        loop++;
        
        if(cpu->pc == 0xC6BC){
            running = false;
        }
        
    }

    // Print nestest results
    printf("NESTEST Results [0x02]: %.2X [0x03]: %.2X", cpu->memory[0x02], cpu->memory[0x03]);

    return 0;
}