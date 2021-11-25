#include "dissassemble.h"

Op* get_op_data(uint8_t opcode){
    // code, label, cycles, mode
    static Op oplist[] = {
        {0x00, "BRK", 7, MODE_IMP},
        {0x01, "ORA", 6, MODE_INX},
        {0x05, "ORA", 3, MODE_ZPG},
        {0x06, "ASL", 5, MODE_ZPG},
        {0x08, "PHP", 3, MODE_IMP},
        {0x09, "ORA", 2, MODE_IMM},
        {0x0a, "ASL", 2, MODE_ACC},
        {0x0d, "ORA", 4, MODE_ABS},
        {0x0e, "ASL", 6, MODE_ABS},
        
        {0x10, "BPL", 2, MODE_REL},
        {0x11, "ORA", 5, MODE_INY},
        {0x15, "ORA", 4, MODE_ZPX},
        {0x16, "ASL", 5, MODE_ZPX},
        {0x18, "CLC", 2, MODE_IMP},
        {0x19, "ORA", 4, MODE_ABY},
        {0x1d, "ORA", 4, MODE_ABX},
        {0x1e, "ASL", 7, MODE_ABX},

        {0x20, "JSR", 6, MODE_ABS},
        {0x21, "AND", 5, MODE_INY},
        {0x24, "BIT", 3, MODE_ZPG},
        {0x25, "AND", 3, MODE_ZPG},
        {0x26, "ROL", 5, MODE_ZPG},
        {0x28, "PLP", 4, MODE_IMP},
        {0x29, "AND", 2, MODE_IMM},
        {0x2a, "ROL", 2, MODE_ACC},
        {0x2c, "BIT", 4, MODE_ABS},
        {0x2d, "AND", 4, MODE_ABS},
        {0x2e, "ROL", 6, MODE_ABS},

        {0x30, "BMI", 2, MODE_REL},
        {0x31, "AND", 5, MODE_INY},
        {0x35, "AND", 4, MODE_ZPX},
        {0x36, "ROL", 6, MODE_ZPX},
        {0x38, "SEC", 2, MODE_IMP},
        {0x39, "AND", 4, MODE_ABY},
        {0x3d, "AND", 4, MODE_ABX},
        {0x3e, "ROL", 7, MODE_ABX},

        {0x40, "RTI", 6, MODE_IMP},
        {0x41, "EOR", 6, MODE_INX},
        {0x45, "EOR", 3, MODE_ZPG},
        {0x46, "LSR", 5, MODE_ZPG},
        {0x48, "PHA", 3, MODE_IMP},
        {0x49, "EOR", 2, MODE_IMM},
        {0x4a, "LSR", 2, MODE_ACC},
        {0x4c, "JMP", 3, MODE_ABS},
        {0x4d, "EOR", 4, MODE_ABS},
        {0x4e, "LSR", 6, MODE_ABS},

        {0x50, "BVC", 2, MODE_REL},
        {0x51, "EOR", 5, MODE_INY},
        {0x55, "EOR", 4, MODE_ZPX},
        {0x56, "LSR", 6, MODE_ZPX},
        {0x58, "CLI", 2, MODE_IMP},
        {0x59, "EOR", 4, MODE_ABY},
        {0x5D, "EOR", 4, MODE_ABX},
        {0x5E, "LSR", 7, MODE_ABX},

        {0x60, "RTS", 6, MODE_IMP},
        {0x61, "ADC", 6, MODE_INX},
        {0x65, "ADC", 3, MODE_ZPG},
        {0x66, "ROR", 5, MODE_ZPG},
        {0x68, "PLA", 4, MODE_IMP},
        {0x69, "ADC", 2, MODE_IMM},
        {0x6a, "ROR", 2, MODE_ACC},
        {0x6c, "JMP", 5, MODE_IND},
        {0x6d, "ADC", 4, MODE_ABS},
        {0x6e, "ROR", 6, MODE_ABS},

        {0x70, "BVS", 2, MODE_REL},
        {0x71, "ADC", 5, MODE_INY},
        {0x75, "ADC", 4, MODE_ZPX},
        {0x76, "ROR", 6, MODE_ZPG},
        {0x78, "SEI", 2, MODE_IMP},
        {0x79, "ADC", 4, MODE_ABS},
        {0x7d, "ADC", 4, MODE_ABX},
        {0x7e, "ROR", 7, MODE_ABX},

        {0x81, "STA", 6, MODE_INX},
        {0x84, "STY", 3, MODE_ZPG},
        {0x85, "STA", 3, MODE_ZPG},
        {0x86, "STX", 3, MODE_ZPG},
        {0x88, "DEY", 2, MODE_IMP},
        {0x8a, "TXA", 2, MODE_IMP},
        {0x8d, "STA", 4, MODE_ABS},
        {0x8e, "STX", 4, MODE_ABS},
        
        {0x90, "BCC", 2, MODE_REL},
        {0x91, "STA", 6, MODE_INY},
        {0x94, "STY", 4, MODE_ZPX},
        {0x95, "STA", 4, MODE_ZPX},
        {0x96, "STX", 4, MODE_ZPY},
        {0x98, "TYA", 2, MODE_IMP},
        {0x99, "STA", 5, MODE_ABY},
        {0x9a, "TXS", 2, MODE_IMP},
        {0x9d, "STA", 5, MODE_ABX},
        
        {0xa0, "LDY", 2, MODE_IMM},
        {0xa1, "LDA", 6, MODE_INX},
        {0xa2, "LDX", 2, MODE_IMM},
        {0xa4, "LDY", 3, MODE_ZPG},
        {0xa5, "LDA", 3, MODE_ZPG},
        {0xa6, "LDX", 3, MODE_ZPG},
        {0xa8, "TAY", 2, MODE_IMP},
        {0xa9, "LDA", 2, MODE_IMM},
        {0xaa, "TAX", 2, MODE_IMP},
        {0xac, "LDY", 4, MODE_ABS},
        {0xad, "LDA", 4, MODE_ABS},
        {0xae, "LDX", 4, MODE_ABS},

        
        {0xb0, "BCS", 2, MODE_REL},
        {0xb1, "LDA", 5, MODE_INY},
        {0xb4, "LDY", 4, MODE_ZPX},
        {0xb5, "LDA", 4, MODE_ZPX},
        {0xb6},
        {0xb8},
        {0xb9, "LDA", 4, MODE_ABY},
        {0xba}
        {0xbc, "LDY", 4, MODE_ABX},
        {0xbd, "LDA", 4, MODE_ABX},
        {0xbe, "LDX", 4, MODE_ABY},

        {0xc0, "CPY", 2, MODE_IMM},
        {0xca, "DEX", 2, MODE_IMP},
        {0xc4, "CPY", 3, MODE_ZPG},
        {0xc5, "CMP", 3, MODE_ZPG},
        {0xc6, "DEC", 5, MODE_ZPG},
        {0xc8, "INY", 2, MODE_IMP},
        {0xc9, "CMP", 2, MODE_IMM},
        {0xcd, "CMP", 4, MODE_ABS},
        {0xce, "DEC", 6, MODE_ABS},

        {0xd0, "BNE", 2, MODE_REL},
        {0xd1, "CMP", 5, MODE_INY},
        {0xd5, "CMP", 4, MODE_ZPX},
        {0xd6, "DEC", 6, MODE_ZPX},
        {0xd8, "CLD", 2, MODE_IMP},
        {0xd9, "CMP", 4, MODE_ABY},
        {0xdd, "CMP", 4, MODE_ABX},
        {0xde, "DEC", 7, MODE_ABX},

        {0xe0, "CPX", 2, MODE_IMM},
        {0xe4, "CPX", 3, MODE_ZPG},
        {0xe5, "SBC", 3, MODE_ZPG},
        {0xeC, "CPX", 4, MODE_ABS},
        {0xe6, "INC", 5, MODE_ZPG},
        {0xe8, "INX", 2, MODE_IMP},
        {0xe9, "SBC", 2, MODE_IMM},
        {0xee, "INC", 6, MODE_ABS},

        {0xf0, "BEQ", 2, MODE_REL},
        {0xf5, "SBC", 4, MODE_ZPX},
        {0xf6, "INC", 6, MODE_ZPX},
        {0xf9, "SBC", 4, MODE_ABY},
        {0xfd, "SBC", 4, MODE_ABX},
        {0xfe, "INC", 7, MODE_ABX}

    };

    Op* found = NULL;

    static uint8_t op_total = sizeof(oplist) / sizeof(oplist[0]);

    for(uint8_t i = 0; i < op_total; i++){
        if(opcode == oplist[i].code){
            found = &oplist[i];
            return found;
        }
    }

    printf("$%x unimplemented.\n", opcode);
    return NULL;
}

void print_dissassembly(CPU* cpu){

    while(cpu->pc < 0xffff){
        uint8_t opcode = cpu->memory[cpu->pc];
        cpu->pc++;

        Op *opdata = get_op_data(opcode);

        if (opdata != NULL){
            // Print opcode memory location and name
            printf("%x\t%s ", cpu->pc - 1, opdata->label);

            // print operand
            switch (opdata->mode){
                case MODE_ABS: printf("$%.4X",     read16(cpu, cpu->pc)); cpu->pc += 2; break;
                case MODE_ABX: printf("$%.4X,X",   read16(cpu, cpu->pc)); cpu->pc += 2; break;
                case MODE_ABY: printf("$%.4X,Y",   read16(cpu, cpu->pc)); cpu->pc += 2; break;
                case MODE_ACC: printf("A"); break;
                case MODE_IMM: printf("#$%.2X",    read8(cpu, cpu->pc)); cpu->pc += 1; break;
                case MODE_IMP: break;
                case MODE_INX: printf("($%.2X,X)", read8(cpu, cpu->pc)); cpu->pc += 1; break;
                case MODE_IND: printf("($%.4X)",   read16(cpu, cpu->pc)); cpu->pc += 2; break;
                case MODE_INY: printf("($%.2X),Y", read8(cpu, cpu->pc)); cpu->pc += 1; break;
                case MODE_REL: printf("$%.2X",     read8(cpu, cpu->pc)); cpu->pc += 1; break;
                case MODE_ZPG: printf("$%.2X",     read8(cpu, cpu->pc)); cpu->pc += 1; break;
                case MODE_ZPX: printf("$%.2X,X",   read8(cpu, cpu->pc)); cpu->pc += 1; break;
                case MODE_ZPY: printf("$%.2X,Y",   read8(cpu, cpu->pc)); cpu->pc += 1; break;
                default: // this should literally never be reached
                    printf("Error: opcode $%x reading invalid addressing mode.", opdata->code);
                    break;
            }

            
        } else{
            // Print memory location and undefined opcode 
            printf("%x\t%s ", cpu->pc - 1, "???");
            break;
        }

        printf("\n");
    }


}