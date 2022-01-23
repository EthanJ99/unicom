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

void BIT(CPU* cpu, uint16_t operand){
    uint8_t val = cpu->a & read8(cpu, operand);

    handle_flag_z(cpu, val);

    // Set N to bit 7 (left-most bit) of val
    if(0x80 & val){
        set_flag(cpu, FLAG_N);
    } else{
        clear_flag(cpu, FLAG_N);
    }

    // Set V to bit 6 of val
    if(0x40 & val){
        set_flag(cpu, FLAG_O);
    } else{
        clear_flag(cpu, FLAG_O);
    }
}

/* ------------------------------------ Arithmetic ------------------------------------ */
void ADC(CPU* cpu, uint16_t operand){
    /* Add with carry
     If the value has overflowed the overflow flag must be set.
     An easy way to check for overflow is to see if the sign has changed
     when it shouldn't have. */
    uint8_t data = cpu->memory[operand];

    // Thanks to calc84maniac from the EmuDev discord for this trick for
    // checking if the overflow flag must be set:
    // We use an int so the value is not limited to 8-bits.
    int signed_result = (int8_t)cpu->a + (int8_t)data + check_flag(cpu, FLAG_C);
    // Then compare to the value if it *were* limited to 8 bits. If they aren't equal there must be some overflow.
    uint8_t overflow = (signed_result != (int8_t)signed_result);

    uint8_t carry = (signed_result > 0xFF); // carry if result over max range
    
    cpu->a = (uint8_t) (signed_result & 0x00FF); // store final result in acc as an 8-bit value again

    if(overflow){
        set_flag(cpu, FLAG_O);
    } else{
        clear_flag(cpu, FLAG_O);
    }

    if(carry){
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }

    handle_flag_n(cpu, cpu->a);
    handle_flag_z(cpu, cpu->a);


}

void SBC(CPU* cpu, uint16_t operand){
    /* Add with carry
     If the value has overflowed the overflow flag must be set.
     An easy way to check for overflow is to see if the sign has changed
     when it shouldn't have. */
    uint8_t data = cpu->memory[operand];

    // Get the complement of the data as we are subtracting
    data = ~data + 1;

    // Thanks to calc84maniac from the EmuDev discord for this trick for
    // checking if the overflow flag must be set:
    // We use an int so the value is not limited to 8-bits.
    int signed_result = (int8_t)cpu->a + (int8_t)data + check_flag(cpu, FLAG_C);
    // Then compare to the value if it *were* limited to 8 bits. If they aren't equal there must be some overflow.
    uint8_t overflow = (signed_result != (int8_t)signed_result);

    uint8_t carry = (signed_result > 0xFF); // carry if result over max range
    
    cpu->a = (uint8_t) (signed_result & 0x00FF); // store final result in acc as an 8-bit value again

    if(overflow){
        set_flag(cpu, FLAG_O);
    } else{
        clear_flag(cpu, FLAG_O);
    }

    if(carry){
        set_flag(cpu, FLAG_C);
    } else{
        clear_flag(cpu, FLAG_C);
    }

    handle_flag_n(cpu, cpu->a);
    handle_flag_z(cpu, cpu->a);
}

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
        uint8_t carry = (cpu->a << 7) & 0x01; 

        // carry <-- accumulator <-- new carry
        cpu->a = (check_flag(cpu, FLAG_C) & 0x01) | (cpu->a << 1);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_n(cpu, cpu->a);
    } else{
        uint16_t data = cpu->memory[operand];
        uint8_t carry = (data << 7) & 0x01; 

        // carry <-- accumulator <-- new carry
        data = (check_flag(cpu, FLAG_C) & 0x01) | (data << 1);

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
        cpu->a = (check_flag(cpu, FLAG_C) << 7) | (cpu->a >> 1);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_n(cpu, cpu->a);
    } else{
        uint16_t data = cpu->memory[operand];
        uint8_t carry = data & 0x01; 

        // carry --> data --> new carry
        data = (check_flag(cpu, FLAG_C) << 7) | (data >> 1);

        if(carry){
            set_flag(cpu, FLAG_C);
        } else{
            clear_flag(cpu, FLAG_C);
        }

        handle_flag_n(cpu, data);
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
}

void JSR(CPU* cpu, uint16_t operand){
    // JSR - Jump to subroutine but save return address on stack

    // First decrement PC
    cpu->pc--;

    // Push current PC to stack, high byte first
    stack_push(cpu, (cpu->pc >> 8) && 0x00FF); // push high
    stack_push(cpu, (cpu->pc) && 0x00FF); // push low
    
    // Set current PC to instruction operand
    cpu->pc = operand;
}

// Return from subroutine - pull PC from stack and increment it, then continue execution
void RTS(CPU* cpu){
    uint8_t pc = stack_pull(cpu);
    cpu->pc = pc + 1;
}

/* ------------------------------------ Branches ------------------------------------ */
void BCC(CPU* cpu, uint16_t branch_addr){
    // branch if carry flag clear
    if(!check_flag(cpu, FLAG_C)){
        cpu->pc = branch_addr;
    }
}

void BCS(CPU* cpu, uint16_t branch_addr){
    // branch if carry flag set
    if(check_flag(cpu, FLAG_C)){
        cpu->pc = branch_addr;
    }
}

void BEQ(CPU* cpu, uint16_t branch_addr){
    // branch if equal/zero flag set
    if(check_flag(cpu, FLAG_Z)){
        cpu->pc = branch_addr;
    }
}

void BMI(CPU* cpu, uint16_t branch_addr){
    // branch if negative flag set
    if(check_flag(cpu, FLAG_N)){
        cpu->pc = branch_addr;
    }
}

void BNE(CPU* cpu, uint16_t branch_addr){
    // branch if not equal/zero flag clear
    if(!check_flag(cpu, FLAG_Z)){
        cpu->pc = branch_addr;
    }
}

void BPL(CPU* cpu, uint16_t branch_addr){
    // Branch if positive (i.e. if Negative flag not set)
    if(!check_flag(cpu, FLAG_N)){
        cpu->pc = branch_addr;
    }
}

void BVC(CPU* cpu, uint16_t branch_addr){
    // branch if overflow flag clear
    if(!check_flag(cpu, FLAG_O)){
        cpu->pc = branch_addr;
    }
}

void BVS(CPU* cpu, uint16_t branch_addr){
    // branch if overflow flag set
    if(check_flag(cpu, FLAG_O)){
        cpu->pc = branch_addr;
    }
}


/* ------------------------------------ Status Flag Changes ------------------------------------ */
void CLC(CPU* cpu){ clear_flag(cpu, FLAG_C); }
void CLD(CPU* cpu){ clear_flag(cpu, FLAG_D); }
void CLI(CPU* cpu){ clear_flag(cpu, FLAG_I); }
void CLV(CPU* cpu){ clear_flag(cpu, FLAG_O); }
void SEC(CPU* cpu){ set_flag(cpu, FLAG_C); }
void SED(CPU* cpu){ set_flag(cpu, FLAG_D); }
void SEI(CPU* cpu){ set_flag(cpu, FLAG_I); }