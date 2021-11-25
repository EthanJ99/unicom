#include "ops.h"
#include "cpu.h"
#include <stdio.h>

/* ---------------------------- Load ---------------------------- */
void LDA(CPU* cpu, uint16_t operand){
    cpu->a = read8(cpu, operand);

    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void LDX(CPU* cpu, uint16_t operand){
    cpu->x = read8(cpu, operand);

    handle_flag_z(cpu, cpu->x);
    handle_flag_n(cpu, cpu->x);
}

void LDY(CPU* cpu, uint16_t operand){
    cpu->y = read8(cpu, operand);

    handle_flag_z(cpu, cpu->y);
    handle_flag_n(cpu, cpu->y);
}

/* ---------------------------- Store ---------------------------- */
void STA(CPU* cpu, uint16_t operand){
    cpu->memory[operand] = cpu->a;
}

void STX(CPU* cpu, uint16_t operand){
    cpu->memory[operand] = cpu->x;
}

void STY(CPU* cpu, uint16_t operand){
    cpu->memory[operand] = cpu->y;
}

/* ---------------------------- Register transfers ---------------------------- */
void TAX(CPU* cpu){
    cpu->x = cpu->a;

    // handle Z/N flags
    handle_flag_z(cpu, cpu->x);
    handle_flag_n(cpu, cpu->x);
}

void TAY(CPU* cpu){
    cpu->y = cpu->a;

    // handle Z/N flags
    handle_flag_z(cpu, cpu->y);
    handle_flag_n(cpu, cpu->y);
}

void TXA(CPU* cpu){
    cpu->a = cpu->x;

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void TYA(CPU* cpu){
    cpu->a = cpu->y;

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

/* ---------------------------- Stack operations ---------------------------- */
void TSX(CPU* cpu){
    cpu->x = cpu->s;

    // handle Z/N flags
    handle_flag_z(cpu, cpu->x);
    handle_flag_n(cpu, cpu->x);
}

void TXS(CPU* cpu){
    cpu->s = cpu->x;
}

void PHA(CPU* cpu){
    stack_push(cpu, cpu->a);
}

void PHP(CPU* cpu){
    stack_push(cpu, cpu->s);
}

void PLA(CPU* cpu){
    cpu->a = stack_pull(cpu);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void PLP(CPU* cpu){
    cpu->p = stack_pull(cpu);
}

/* ---------------------------- Logical ---------------------------- */

/* OKAY YOU MIGHT NEED TO BE DOING MEMORY READS.
REMEMBER YOU HAVE A read8() FUNCTION ! ! ! */

void AND(CPU* cpu, uint16_t operand){
    cpu->a &= read8(cpu, operand);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void EOR(CPU* cpu, uint16_t operand){
    cpu->a ^= read8(cpu, operand);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void ORA(CPU* cpu, uint16_t operand){
    cpu->a |= read8(cpu, operand);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

/* void BIT(CPU* cpu, uint16_t operand){
    uint8_t val = cpu->a & read8(cpu, operand);

    handle_flag_z(cpu, val);

    // Set N to bit 7 (left-most bit) of val
    

    // Set V to bit 6 of val
    
} */

/* ------------------------------------ Arithmetic ------------------------------------ */
/*void ADC(CPU* cpu, uint16_t operand){
    cpu->a += read8(cpu, operand) + 
}*/

/*
void SBC(CPU* cpu, uint16_t operand){

}*/

void CMP(CPU* cpu, uint16_t operand){
    uint8_t val = cpu->a - read8(cpu, operand);

    // Set carry flag if Accumulator minus the value in location operand >= 0
    if( val >= 0 ){
        set_flag(cpu, FLAG_C);
    }
    
    // Set zero flag if Accumulator = the value in location operand
    handle_flag_z(cpu, val);

    // Set N flag if bit 7 of val is set
    handle_flag_n(cpu, val);

}

void CPX(CPU* cpu, uint16_t operand){
    uint8_t val = cpu->x - read8(cpu, operand);

    // Set carry flag if X minus the value in location operand >= 0
    if( val >= 0 ){
        set_flag(cpu, FLAG_C);
    }
    
    // Set zero flag if X = the value in location operand
    handle_flag_z(cpu, val);

    // Set N flag if bit 7 of val is set
    handle_flag_n(cpu, val);

}

void CPY(CPU* cpu, uint16_t operand){
    uint8_t val = cpu->y - read8(cpu, operand);

    // Set carry flag if Y minus the value in location operand >= 0
    if( val >= 0 ){
        set_flag(cpu, FLAG_C);
    }
    
    // Set zero flag if Y = the value in location operand
    handle_flag_z(cpu, val);

    // Set N flag if bit 7 of val is set
    handle_flag_n(cpu, val);

}

/* ------------------------------ Increments & Decrements ------------------------------ */
void INC(CPU* cpu, uint16_t operand){
    cpu->memory[operand]++;

    handle_flag_z(cpu, cpu->memory[operand]);
    handle_flag_n(cpu, cpu->memory[operand]);
}

void INX(CPU* cpu){
    cpu->x++;

    handle_flag_z(cpu, cpu->x);
    handle_flag_n(cpu, cpu->x);
}

void INY(CPU* cpu){
    cpu->y++;

    handle_flag_z(cpu, cpu->y);
    handle_flag_n(cpu, cpu->y);
}

void DEC(CPU* cpu, uint16_t operand){
    cpu->memory[operand]--;

    handle_flag_z(cpu, cpu->memory[operand]);
    handle_flag_n(cpu, cpu->memory[operand]);
}

void DEX(CPU* cpu){
    cpu->x--;

    handle_flag_z(cpu, cpu->x);
    handle_flag_n(cpu, cpu->x);
}

void DEY(CPU* cpu){
    cpu->y--;

    handle_flag_z(cpu, cpu->y);
    handle_flag_n(cpu, cpu->y);
}

/* ------------------------------------ Shifts ------------------------------------ */
void ASL(CPU* cpu, uint16_t operand){
    clear_flag(cpu, FLAG_C);
    cpu->p |= cpu->memory[operand] & 0x80;
    cpu->memory[operand] = cpu->memory[operand] << 2;

}