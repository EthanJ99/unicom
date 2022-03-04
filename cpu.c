#include "cpu.h"

#include <stdlib.h>
#include <string.h>

void cpu_init(CPU* cpu){
    memset(cpu->memory, 0, sizeof(cpu->memory));
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->pc = 0;
    cpu->s = 0xFD;
    cpu->p = 0x24;

    cpu->cycles = 7;
}

/* ---------------------------- Miscellaneous functions ---------------------------- */
uint16_t byte_swap(uint16_t val){
    return (val >> 8) | (val << 8);
}

uint8_t read8(CPU* cpu, uint16_t addr){
    return cpu->memory[addr];
}

uint16_t read16(CPU* cpu, uint16_t addr){
    uint8_t high = read8(cpu, addr);
    uint8_t low = read8(cpu, addr + 1);
    uint16_t val = (high << 8) | low;

    return byte_swap(val);
    
}

/* ------------------------------------ Flag handling ------------------------------------ */
// ORs a flag's bit in the P register with 1 to set it
void set_flag(CPU* cpu, uint8_t flag){
    cpu->p |= 0x1 << flag;
}

// ANDs a flag's bit in the P register with 1, then inverts the result, to clear it
void clear_flag(CPU* cpu, uint8_t flag){
    cpu->p &= ~(0x1 << flag);
}

// Shifts the flag's bit in the P register down to bit 0 and ANDs it with 1 to get its status (1 or 0).
bool check_flag(CPU* cpu, uint8_t flag){
    return (cpu->p >> flag) & 0x1;
}

// Checks if a value is 0 and sets the Z flag if so.
void handle_flag_z(CPU* cpu, uint8_t val){
    if(val == 0){
        set_flag(cpu, FLAG_Z);
    } else{
        clear_flag(cpu, FLAG_Z);
    }
}

// Checks if a value is negative (bit 7 set) and sets the N flag if so.
void handle_flag_n(CPU* cpu, uint8_t val){
    //printf("\nhandle_flag_n val=%.2x out=%.2X\t", val, val & 0x80);
    if( val & 0x80 ){ // 0x80 == b10000000
        //printf("SET flag N");
        set_flag(cpu, FLAG_N);
    } else{
        //printf("CLEAR flag N");
        clear_flag(cpu, FLAG_N);
    }
    //printf("\n");
}

/* ----------------------------------- Stack handling ----------------------------------- */
void stack_push(CPU* cpu, uint8_t val){
    cpu->memory[0x0100 + cpu->s] = val;
    cpu->s--;
}

uint8_t stack_pull(CPU* cpu){
    cpu->s++;
    uint8_t val = cpu->memory[0x0100 + cpu->s];
    /*printf("\nPulling from stack: %.2X. Prev stack val: %.2X Next stack val: %.2X\n",
        cpu->memory[0x0100 + cpu->s], cpu->memory[0x0100 + cpu->s - 1], cpu->memory[0x0100 + cpu->s + 1]);*/
    
    return val;
}

/* ----------------------------------- Addressing modes ----------------------------------- */
// Absolute
uint16_t addr_abs(CPU* cpu){
    cpu->pc += 2;
    return cpu->memory[cpu->pc - 2] | cpu->memory[cpu->pc - 1] << 8;
}

// Absolute (X indexed)
uint16_t addr_abx(CPU* cpu){
    uint16_t fetched = addr_abs(cpu);
    uint16_t addr = fetched + cpu->x;

    if( (fetched & 0xFF00) != (addr & 0xFF00) ){
        cpu->addr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
    }

    return addr;
}

// Absolute (Y indexed)
uint16_t addr_aby(CPU* cpu){
    uint16_t fetched = addr_abs(cpu);
    uint16_t addr = fetched + cpu->y;

    if( (fetched & 0xFF00) != (addr & 0xFF00) ){
        cpu->addr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
    }

    return addr;
}

// Accumulator
// This is a special case - rather than returning an address, it returns the accumulator wrapped up in 16-bits
// This is only used by logical shift/rotate instructions
uint16_t addr_acc(CPU* cpu){
   return (uint16_t)cpu->a & 0x00FF; 
}

// Immediate
uint16_t addr_imm(CPU* cpu){
    //printf("\naddr: %.4X val: %.2X\n", cpu->pc + 1, cpu->memory[cpu->pc + 1]);
    return cpu->pc++;
}

// Indirect
uint16_t addr_ind(CPU* cpu){
    // Indirect reads an address from memory, which is used as a pointer to the true address
    // that will be used by the instruction.

    // Read low/high bytes of pointer address separately
    uint8_t low = read8(cpu, cpu->pc);
    cpu->pc++;
    uint8_t high = read8(cpu, cpu->pc);
    cpu->pc++;

    // Combine into full pointer
    uint16_t addr = (uint16_t)low | ((uint16_t)high << 8);
    
    // The CPU has a bug: If the low byte falls on the boundary of an (8-bit) page in memory,
    // it wraps around when we read the high byte (i.e. if we're reading low from 0x02FF,
    // we will read the high byte from 0x0200 instead of 0x300 as expected).
    
    // I want it known to whoever reads this code that trying to debug this took over a week and nearly made
    // me cry multiple times. I then googled it and it turns out its a known problem with a disgustingly simple
    // solution. God bless the internet (despite its many crimes).
    if(low == 0xFF){
        low = read8(cpu, addr);
        high = read8(cpu, addr & 0xFF00); // wrap round back to the start of the page
        addr = (uint16_t)low | ((uint16_t)high << 8);
    } else{
        low = read8(cpu, addr);
        high = read8(cpu, addr + 1); // don't wrap round
        addr = (uint16_t)low | ((uint16_t)high << 8);
    }

    return addr;

}

// Indirect (X indexed)
uint16_t addr_inx(CPU* cpu){
    uint16_t addr;
    uint8_t fetched = read8(cpu, cpu->pc);
    cpu->pc++;

    // Read low and high bytes separately. 
    // Address wraps around at 0xFF back to 0x00.
    uint16_t low = read8(cpu, (fetched + (uint16_t)cpu->x) & 0x00FF);
    uint16_t high = read8(cpu, (fetched + (uint16_t)cpu->x + 1) & 0x00FF);

    addr = (high << 8) | low;
    
    return addr;
}

// Indirect (Y indexed)
uint16_t addr_iny(CPU* cpu){
    uint16_t addr;
    uint16_t fetched = read8(cpu, cpu->pc);
    cpu->pc++;

    // Read low and high bytes separately.
    uint16_t low = read8(cpu, fetched & 0x00FF);
    uint16_t high = read8(cpu, (fetched + 1) & 0x00FF);

    addr = (high << 8) | low;
    

    addr += cpu->y;

    if( (addr & 0xFF00) != (high << 8) ){
        cpu->addr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
    }
    return addr;
}

// Relative - adjusts PC by a given signed +/- offset
uint16_t addr_rel(CPU* cpu){
    int8_t offset = cpu->memory[cpu->pc++];
    
    return cpu->pc + offset;
}

// Zero Page
uint8_t addr_zpg(CPU* cpu){
    uint16_t addr = cpu->memory[cpu->pc++] & 0x00FF;
    return addr;
}

// Zero Page (X indexed)
uint8_t addr_zpx(CPU* cpu){
    uint16_t addr = (cpu->memory[cpu->pc++] + cpu->x) & 0x00FF;
    return addr;
}

// Zero Page (Y indexed)
uint8_t addr_zpy(CPU* cpu){
    uint16_t addr = (cpu->memory[cpu->pc++] + cpu->y) & 0x00FF;
    return addr;
}

// REPLACE THIS you fool. Keep the array, fill in the blanks with NULL (or a filler blank op), then just index the array
// The function is totally unnecessary
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
        {0x16, "ASL", 6, MODE_ZPX},
        {0x18, "CLC", 2, MODE_IMP},
        {0x19, "ORA", 4, MODE_ABY},
        {0x1d, "ORA", 4, MODE_ABX},
        {0x1e, "ASL", 7, MODE_ABX},

        {0x20, "JSR", 6, MODE_ABS},
        {0x21, "AND", 6, MODE_INX},
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
        {0x3f, "RLA", 7, MODE_ABX},

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
        {0x8c, "STY", 4, MODE_ABS},
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
        {0xb6, "LDX", 4, MODE_ZPY},
        {0xb8, "CLV", 2, MODE_IMP},
        {0xb9, "LDA", 4, MODE_ABY},
        {0xba, "TSX", 2, MODE_IMP},
        {0xbc, "LDY", 4, MODE_ABX},
        {0xbd, "LDA", 4, MODE_ABX},
        {0xbe, "LDX", 4, MODE_ABY},

        {0xc0, "CPY", 2, MODE_IMM},
        {0xc1, "CMP", 6, MODE_INX},
        {0xc4, "CPY", 3, MODE_ZPG},
        {0xc5, "CMP", 3, MODE_ZPG},
        {0xc6, "DEC", 5, MODE_ZPG},
        {0xc8, "INY", 2, MODE_IMP},
        {0xc9, "CMP", 2, MODE_IMM},
        {0xca, "DEX", 2, MODE_IMP},
        {0xcc, "CPY", 4, MODE_ABS},
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
        {0xe1, "SBC", 6, MODE_INX},
        {0xe4, "CPX", 3, MODE_ZPG},
        {0xe5, "SBC", 3, MODE_ZPG},
        {0xe6, "INC", 5, MODE_ZPG},
        {0xec, "CPX", 4, MODE_ABS},
        {0xe6, "INC", 5, MODE_ZPG},
        {0xe8, "INX", 2, MODE_IMP},
        {0xe9, "SBC", 2, MODE_IMM},
        {0xea, "NOP", 2, MODE_IMP},
        {0xec, "CPX", 4, MODE_ABS},
        {0xed, "SBC", 4, MODE_ABS},
        {0xee, "INC", 6, MODE_ABS},

        {0xf0, "BEQ", 2, MODE_REL},
        {0xf1, "SBC", 5, MODE_INY},
        {0xf5, "SBC", 4, MODE_ZPX},
        {0xf6, "INC", 6, MODE_ZPX},
        {0xf8, "SED", 2, MODE_IMP},
        {0xf9, "SBC", 4, MODE_ABY},
        {0xfd, "SBC", 4, MODE_ABX},
        {0xfe, "INC", 7, MODE_ABX}

    };

    static Op invalid_instruction = {0xFF, "XXX", 0, MODE_IMP};

    Op* found = NULL;

    static uint8_t op_total = sizeof(oplist) / sizeof(oplist[0]);

    for(uint8_t i = 0; i < op_total; i++){
        if(opcode == oplist[i].code){
            found = &oplist[i];
            return found;
        }
    }

    // printf("$%X unimplemented.\n", opcode);
    return &invalid_instruction;
}

void print_disassembly(CPU* cpu, bool dump){
    /*if(dump){
        time_t raw_time;
        struct tm *info;
        time( &raw_time );

        info = localtime( &raw_time );
    }*/

    bool running = true;

    while(running){
        uint8_t opcode = cpu->memory[cpu->pc];
        cpu->pc++;

        Op *opdata = get_op_data(opcode);

        if (opdata != NULL){
            // Print opcode memory location and name
            printf("%X\t%s ", cpu->pc - 1, opdata->label);

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
                    printf("Error: opcode $%X reading invalid addressing mode.", opdata->code);
                    break;
            }

            
        } else{
            // Print memory location and undefined opcode 
            printf("%X\t%s ", cpu->pc - 1, "???");
            //break;
        }

        // Stop if we reach the end of memory
        if(cpu->pc >= 0xffff || cpu->pc == 0x00){
            running = false;
        }

        printf("\n");
    }


}

void disassemble_op(CPU* cpu, Op* op){
    // Print opcode memory location
    printf("%X\t", cpu->pc);
    
    // Print raw opcode and instruction values as hex
    printf("%.2X ", op->code);
    switch (op->mode){
        case MODE_ABS: printf("%.2X %.2X",  read8(cpu, cpu->pc + 1), read8(cpu, cpu->pc + 2));  break;
        case MODE_ABX: printf("%.2X %.2X",  read8(cpu, cpu->pc + 1), read8(cpu, cpu->pc + 2));  break;
        case MODE_ABY: printf("%.2X %.2X",  read8(cpu, cpu->pc + 1), read8(cpu, cpu->pc + 2));  break;
        case MODE_IMM: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        case MODE_ACC:
        case MODE_IMP: printf("        ");                                                      break;
        case MODE_INX: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        case MODE_IND: printf("%.2X %.2X",  read8(cpu, cpu->pc + 1), read8(cpu, cpu->pc + 2));  break;
        case MODE_INY: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        case MODE_REL: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        case MODE_ZPG: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        case MODE_ZPX: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        case MODE_ZPY: printf("%.2X   ",    read8(cpu, cpu->pc + 1));                           break;
        default: // this should literally never be reached
            printf("\nError: opcode $%.X reading invalid addressing mode.\n", op->code);
            break;
    }
    printf("\t");
    
    printf("%s ", op->label);
    // Print assembly
    switch (op->mode){
        case MODE_ABS: printf("$%.4X    ",     read16(cpu, cpu->pc + 1)); break;
        case MODE_ABX: printf("$%.4X,X  ",   read16(cpu, cpu->pc + 1)); break;
        case MODE_ABY: printf("$%.4X,Y  ",   read16(cpu, cpu->pc + 1)); break;
        case MODE_ACC: printf("A        ");                                   break;
        case MODE_IMM: printf("#$%.2X   ",    read8(cpu, cpu->pc + 1));  break;
        case MODE_IMP: printf("         ");                            break;
        case MODE_INX: printf("($%.2X,X) ", read8(cpu, cpu->pc + 1));  break;
        case MODE_IND: printf("($%.4X)  ",   read16(cpu, cpu->pc + 1)); break;
        case MODE_INY: printf("($%.2X),Y", read8(cpu, cpu->pc + 1));  break;
        case MODE_REL: printf("$%.4X    ",    cpu->pc + read8(cpu, cpu->pc + 1) + 2);  break;
        case MODE_ZPG: printf("$%.2X    ",     read8(cpu, cpu->pc + 1));  break;
        case MODE_ZPX: printf("$%.2X,X  ",   read8(cpu, cpu->pc + 1));  break;
        case MODE_ZPY: printf("$%.2X,Y  ",   read8(cpu, cpu->pc + 1));  break;
        default: // this should literally never be reached
            printf("Error: opcode $%.X reading invalid addressing mode.", op->code);
            break;
    }

    fflush(stdout);
    //printf("\n");


}