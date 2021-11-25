#include "cpu.h"

#include <stdlib.h>
#include <string.h>

CPU* cpu_init(void){
    CPU* cpu = malloc(sizeof(CPU));

    memset(cpu->memory, 0, sizeof(cpu->memory));
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    cpu->pc = 0;
    cpu->s = 0;
    cpu->p = 0;

    return cpu;
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
    }
}

// Checks if a value is negative (bit 7 set) and sets the N flag is so.
void handle_flag_n(CPU* cpu, uint8_t val){
    if( ((val >> 7) & 1) == 1 ){
        set_flag(cpu, FLAG_N);
    }
}

/* ----------------------------------- Stack handling ----------------------------------- */
void stack_push(CPU* cpu, uint8_t val){
    cpu->memory[0x0100 + cpu->s] = val;
    cpu->s--;
}

uint8_t stack_pull(CPU* cpu){
    uint8_t val = cpu->memory[0x0100 + cpu->s];
    cpu->s++;
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
    return addr_abs(cpu) + cpu->x;

    // Need to account for page crossing
}

// Absolute (Y indexed)
uint16_t addr_aby(CPU* cpu){
    return addr_abs(cpu) + cpu->y;

    // Need to account for page crossing
}

// Immediate
uint16_t addr_imm(CPU* cpu){ return cpu->pc++; }

// Indirect
uint16_t addr_ind(CPU* cpu){
    uint16_t addr_1 = addr_abs(cpu);
    uint16_t addr_2 = cpu->memory[addr_1] | cpu->memory[addr_1 + 1] << 8;
    return addr_2;
}

// Indirect (X indexed)
uint16_t addr_inx(CPU* cpu){ 
    uint8_t addr_1 = (cpu->memory[cpu->pc++] + cpu->x) % 0x100;
    uint8_t addr_2 = cpu->memory[addr_1] | cpu->memory[addr_1 + 1] << 8;
    return addr_2;
}

// Indirect (Y indexed)
uint16_t addr_iny(CPU* cpu){
    uint8_t addr_1 = cpu->memory[cpu->pc++];
    uint16_t addr_2 = cpu->memory[addr_1] | cpu->memory[addr_1 + 1] << 8;
    return addr_2 + cpu->y;
}

// RELATIVE ADDRESSING


// Zero Page
uint8_t addr_zpg(CPU* cpu){ return cpu->memory[cpu->pc++]; }

// Zero Page (X indexed)
uint8_t addr_zpx(CPU* cpu){ return (cpu->memory[cpu->pc++] + cpu->x) % 0x100; }

// Zero Page (Y indexed)
uint8_t addr_zpy(CPU* cpu){ return (cpu->memory[cpu->pc++] + cpu->y) % 0x100; }