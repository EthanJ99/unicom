#include "ops.h"
#include "cpu.h"
#include <stdio.h>

/* ---------------------------- Load ---------------------------- */
void LDA(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    cpu->a = read8(cpu, operand);
    printf("[LDA] addr: %.4X a: %.2X", operand, cpu->a);

    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void LDX(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    cpu->x = read8(cpu, operand);
    printf("[LDX] addr: %.4X a: %.2X", operand, cpu->a);
    handle_flag_z(cpu, cpu->x);
    handle_flag_n(cpu, cpu->x);
}

void LDY(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    cpu->y = read8(cpu, operand);
    printf("[LDY] addr: %.4X a: %.2X", operand, cpu->a);
    handle_flag_z(cpu, cpu->y);
    handle_flag_n(cpu, cpu->y);
}

/* ---------------------------- Store ---------------------------- */
void STA(CPU* cpu, uint16_t operand){
    //printf("\n\nSTA P before: %.2x\n", cpu->p);
    
    cpu->memory[operand] = cpu->a;
    printf("[STA] addr: %.4X val at addr: %.2X a: %.2X", operand, cpu->memory[operand], cpu->a);
    //printf("STA P after: %.2x\n\n", cpu->p);
}

void STX(CPU* cpu, uint16_t operand){
    printf("[STA] addr: %.4X a: %.2X", operand, cpu->a);
    cpu->memory[operand] = cpu->x;
}

void STY(CPU* cpu, uint16_t operand){
    printf("[STA] addr: %.4X a: %.2X", operand, cpu->a);
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
    stack_push(cpu, cpu->p);
}

void PLA(CPU* cpu){
    //printf("\n\n PLA P before: %.2x\n", cpu->p);
    cpu->a = stack_pull(cpu);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
    //printf(" PLA P after: %.2x\n", cpu->p);
}

void PLP(CPU* cpu){
    cpu->p = stack_pull(cpu);
}

/* ---------------------------- Logical ---------------------------- */
void AND(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    //printf("\n\n AND P before: %.2x\n", cpu->p);
    cpu->a &= read8(cpu, operand);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
    //printf(" AND P after: %.2x\n", cpu->p);
}

void EOR(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes
    cpu->a ^= read8(cpu, operand);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void ORA(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    cpu->a |= read8(cpu, operand);

    // handle Z/N flags
    handle_flag_z(cpu, cpu->a);
    handle_flag_n(cpu, cpu->a);
}

void BIT(CPU* cpu, uint16_t operand){
    uint8_t pulled = read8(cpu, operand);

    handle_flag_z(cpu, cpu->a & pulled);

    // Set N to bit 7 (left-most bit) of pulled value
    if(0x80 & pulled){
        set_flag(cpu, FLAG_N);
    } else{
        clear_flag(cpu, FLAG_N);
    }

    // Set V to bit 6 of pulled value
    if(0x40 & pulled){
        set_flag(cpu, FLAG_V);
    } else{
        clear_flag(cpu, FLAG_V);
    }
}

/* ------------------------------------ Arithmetic ------------------------------------ */
// Add with Carry
void ADC(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    uint8_t fetched = read8(cpu, operand);

    // Add value to accumulator, accounting for carry
    uint16_t sum = (uint16_t)cpu->a + (uint16_t)fetched + (uint16_t)check_flag(cpu, FLAG_C);

    // Set carry flag if value > 255
    if(sum > 0xFF) {
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }

    // Set zero flag if result is zero
    if((sum & 0x00FF) == 0){
        set_flag(cpu, FLAG_Z);
    } else{
        clear_flag(cpu, FLAG_Z);
    }

    // Set overflow flag if sign has changed incorrectly
    // Sources:
    // - http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
    // - https://forums.nesdev.org/viewtopic.php?t=6331
    // - https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp
    if((~((uint16_t)cpu->a ^ (uint16_t)fetched) & ((uint16_t)cpu->a ^ (uint16_t)sum)) & 0x0080){
        set_flag(cpu, FLAG_V);
    } else{
        clear_flag(cpu, FLAG_V);
    }

    // Set Negative flag to MSB of result
    handle_flag_n(cpu, sum);

    // Set acc to final result (convert back to 8bit)
    cpu->a = sum & 0x00ff;

}

// Subtract with carry
void SBC(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes
    
    uint8_t temp = read8(cpu, operand);

    // Invert the value to allow subtraction
    uint16_t fetched = ((uint16_t)temp ^ 0x00FF);

    // Add value to accumulator, accounting for carry
    uint16_t sum = (uint16_t)cpu->a + (uint16_t)fetched + (uint16_t)check_flag(cpu, FLAG_C);

    // Set carry flag if value > 255
    if(sum > 0xFF) {
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }

    // Set zero flag if result is zero
    if((sum & 0x00FF) == 0){
        set_flag(cpu, FLAG_Z);
    } else{
        clear_flag(cpu, FLAG_Z);
    }

    // Set overflow flag if sign has changed incorrectly
    if((~((uint16_t)cpu->a ^ (uint16_t)fetched) & ((uint16_t)cpu->a ^ (uint16_t)sum)) & 0x0080){
        set_flag(cpu, FLAG_V);
    } else{
        clear_flag(cpu, FLAG_V);
    }

    // Set Negative flag to MSB of result
    handle_flag_n(cpu, sum);

    // Set acc to final result (convert back to 8bit)
    cpu->a = sum & 0x00ff;

    // Handle additional clock cycle

}

void CMP(CPU* cpu, uint16_t operand){
    cpu->instr_extra_cycle = true; // inform CPU this instruction takes +1 cycles under certain addr. modes

    //printf("\n\n CMP P before: %.2x\n", cpu->p);
    uint8_t pulled = read8(cpu, operand);

    uint16_t val = (uint16_t)cpu->a - (uint16_t)pulled;

    // Set carry flag if Accumulator greater than read value
    if( cpu->a >= pulled ){
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }
    
    // Set zero flag if Accumulator == the value in location operand
    handle_flag_z(cpu, (val & 0x00FF));

    // Set N flag if bit 7 of val is set (i.e. val is signed)
    handle_flag_n(cpu, val);
    //printf(" CMP P after: %.2x\n", cpu->p);

}

void CPX(CPU* cpu, uint16_t operand){
    uint8_t pulled = read8(cpu, operand);
    uint8_t val = cpu->x - pulled;

    // Set carry flag if X greater than read value
    if( cpu->x >= pulled ){
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }
    
    // Set zero flag if X = the value in location operand
    handle_flag_z(cpu, val);

    // Set N flag if bit 7 of val is set (i.e. val is signed)
    handle_flag_n(cpu, val);

}

void CPY(CPU* cpu, uint16_t operand){
    uint8_t pulled = read8(cpu, operand);
    uint8_t val = cpu->y - pulled;

    // Set carry flag if Y greater than read value
    if( cpu->y >= pulled ){
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }
    
    // Set zero flag if Y = the value in location operand
    handle_flag_z(cpu, val);

    // Set N flag if bit 7 of val is set (i.e. val is signed)
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
void ASL(CPU* cpu, uint16_t operand, Mode addr_mode){
    if(addr_mode == MODE_ACC){
        uint8_t carry = cpu->a & 0x80;
        cpu->a = cpu->a << 1;
        handle_flag_n(cpu, cpu->a);
        handle_flag_z(cpu, cpu->a);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }
    } else{
        uint8_t carry = cpu->memory[operand] & 0x80;
        cpu->memory[operand] = cpu->memory[operand] << 1;
        handle_flag_n(cpu, cpu->memory[operand]);
        handle_flag_z(cpu, cpu->memory[operand]);
        
        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }
    }
    

}

void LSR(CPU* cpu, uint16_t operand, Mode addr_mode){
    if(addr_mode == MODE_ACC){
        uint8_t carry = cpu->a & 0x01;
        cpu->a = cpu->a >> 1;

        handle_flag_n(cpu, cpu->a);
        handle_flag_z(cpu, cpu->a);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }
    } else{
        uint8_t carry = cpu->memory[operand] & 0x01;
        cpu->memory[operand] = cpu->memory[operand] >> 1;

        handle_flag_n(cpu, cpu->memory[operand]);
        handle_flag_z(cpu, cpu->memory[operand]);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }
    }
}

void ROL(CPU* cpu, uint16_t operand, Mode addr_mode){
    if(addr_mode == MODE_ACC){
        uint8_t carry = cpu->a & 0x80; 

        // carry <-- accumulator <-- new carry
        cpu->a = ((uint8_t)check_flag(cpu, FLAG_C)) | (cpu->a << 1);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_n(cpu, cpu->a);
    } else{
        uint16_t data = cpu->memory[operand];
        uint8_t carry = data & 0x80; 

        // carry <-- accumulator <-- new carry
        data = ((uint8_t)check_flag(cpu, FLAG_C)) | (data << 1);
        cpu->memory[operand] = (uint8_t)data;

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_n(cpu, data);
    }
}

void ROR(CPU* cpu, uint16_t operand, Mode addr_mode){
    if(addr_mode == MODE_ACC){
        uint8_t carry = cpu->a & 0x01; 

        // carry --> accumulator --> new carry
        cpu->a = ((uint8_t)check_flag(cpu, FLAG_C) << 7) | (cpu->a >> 1) & 0x00FF;

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_z(cpu, cpu->a);
        handle_flag_n(cpu, cpu->a);
    } else{
        uint16_t data = cpu->memory[operand];
        uint8_t carry = data & 0x01; 

        // carry --> data --> new carry
        data = (check_flag(cpu, FLAG_C) << 7) | (data >> 1);
        cpu->memory[operand] = (uint8_t)data & 0x00FF;

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_z(cpu, data & 0x00FF);
        handle_flag_n(cpu, data & 0x00FF);
    }
}

/* ------------------------------------ System functions ------------------------------------ */
void BRK(CPU* cpu){
    // Push return value to stack (val == PC + 2)
    stack_push(cpu, cpu->pc + 2);

    // Push the status register to the stack with the break flag set
    set_flag(cpu, FLAG_B);
    stack_push(cpu, cpu->p);
}

void NOP(){
    return;
}

void RTI(CPU* cpu){
    // Pull status register
    cpu->p = stack_pull(cpu);

    // Pull PC
    cpu->pc = stack_pull(cpu) | (stack_pull(cpu) << 8);
}

/* ------------------------------------ Jumps and Calls ------------------------------------ */
// Jump to a given memory address
void JMP(CPU* cpu, uint16_t operand){
    cpu->pc = operand;
    printf("[JMP] addr: %.4X val: %.2X PC: %.2X", operand, cpu->memory[operand], cpu->pc);
    
}

void JSR(CPU* cpu, uint16_t operand){
    // JSR - Jump to subroutine but save return address on stack

    // First decrement PC
    cpu->pc--;

    // Push current PC to stack, high byte first
    uint8_t high = (cpu->pc >> 8) & 0x00FF;
    stack_push(cpu, high); // push high

    uint8_t low = (cpu->pc) & 0x00FF;
    stack_push(cpu, low); // push low
    
    // Set current PC to instruction operand
    cpu->pc = operand;
}

// Return from subroutine - pull PC from stack and increment it, then continue execution
void RTS(CPU* cpu){
    // Pull low and high bytes separately
    uint8_t low = stack_pull(cpu);
    uint8_t high = stack_pull(cpu);

    uint16_t addr = (high << 8) | (low & 0xff);

    cpu->pc = addr + 1;
}

/* ------------------------------------ Branches ------------------------------------ */
void BCC(CPU* cpu, uint16_t branch_addr){
    // branch if carry flag clear
    if(check_flag(cpu, FLAG_C) == false){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        
        cpu->pc = branch_addr;
    }
}

void BCS(CPU* cpu, uint16_t branch_addr){
    // branch if carry flag set
    if(check_flag(cpu, FLAG_C)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    } 
}

void BEQ(CPU* cpu, uint16_t branch_addr){
    // branch if equal/zero flag set
    if(check_flag(cpu, FLAG_Z)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    }
}

void BMI(CPU* cpu, uint16_t branch_addr){
    // branch if negative flag set
    if(check_flag(cpu, FLAG_N)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    }
}

void BNE(CPU* cpu, uint16_t branch_addr){
    // branch if not equal/zero flag clear
    if(!check_flag(cpu, FLAG_Z)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    }
}

void BPL(CPU* cpu, uint16_t branch_addr){
    // Branch if positive (i.e. if Negative flag not set)
    if(!check_flag(cpu, FLAG_N)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    }
}

void BVC(CPU* cpu, uint16_t branch_addr){
    // branch if overflow flag clear
    if(!check_flag(cpu, FLAG_V)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    }
}

void BVS(CPU* cpu, uint16_t branch_addr){
    // branch if overflow flag set
    if(check_flag(cpu, FLAG_V)){
        if( (cpu->pc & 0xFF00) != (branch_addr & 0xFF00) ){
            cpu->instr_extra_cycle = true; // alert CPU to +1 cycle if page boundary crossed
        }

        cpu->cycles++; // +1 cycle if branch succeeds
        cpu->pc = branch_addr;
    }
}


/* ------------------------------------ Status Flag Changes ------------------------------------ */
void CLC(CPU* cpu){ clear_flag(cpu, FLAG_C); }
void CLD(CPU* cpu){ clear_flag(cpu, FLAG_D); }
void CLI(CPU* cpu){ clear_flag(cpu, FLAG_I); }
void CLV(CPU* cpu){ clear_flag(cpu, FLAG_V); }
void SEC(CPU* cpu){ set_flag(cpu, FLAG_C); }
void SED(CPU* cpu){ set_flag(cpu, FLAG_D); }
void SEI(CPU* cpu){ set_flag(cpu, FLAG_I); }