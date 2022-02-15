#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    MODE_ABS,
    MODE_ABX,
    MODE_ABY,
    MODE_ACC,
    MODE_IMM,
    MODE_IMP,
    MODE_INX,
    MODE_IND,
    MODE_INY,
    MODE_REL,
    MODE_ZPG,
    MODE_ZPX,
    MODE_ZPY
} Mode;

typedef struct Op{
    uint8_t code;
    char* label;
    uint8_t cycles;
    Mode mode;
} Op;

typedef struct CPU{
    uint8_t memory[0xffff];

    // Accumulator
    uint8_t a;

    // Indexes (used for loop counters, branch instrs., etc)
    uint8_t x;
    uint8_t y;

    // Program Counter
    uint16_t pc;

    // Stack Pointer
    uint8_t s;

    // Processor Status Register 
    uint8_t p;
} CPU;

// Return a CPU with some preset values for its registers and memory
CPU* cpu_init(void);

// Swaps two bytes in a 16-bit integer
uint16_t byte_swap(uint16_t val);

// Read a byte from CPUs memory from the given address
uint8_t read8(CPU* cpu, uint16_t addr);
uint16_t read16(CPU* cpu, uint16_t addr);

Op* get_op_data(uint8_t opcode);

// Original Disassembly function - loops through loaded ROM and prints full disassembly.
void print_dissassembly(CPU* cpu, bool dump);

// Disassembles a single line/instruction
void disassemble_op(CPU* cpu, Op* op);

/* ------------------------------------ Flag handling ------------------------------------ */

/* 7  bit  0
   ---- ----
   NVss DIZC

 *  (N)egative - Set if bit 7 of last operation set to 1
 * O(v)erflow - Set if result of last op gave invalid twos complement result (i.e. 64+64=-128)
 *  (B)reak - Set when BRK instr. executed and interrupt has been generated to process it.
 *  (D)ecimal - (NOT USED ON NES) Processor will use BCD (instead of twos comp ???)
 *  (I)nterrupt Disable - Forces processor to ignore interrupts if set
 *  (Z)ero - set if result of last operation was 0.
 *  (C)arry - set if last operation caused overflow from bit 7 or underflow from bit 0. */

// Define a value (1 bit of the status register byte) for each flag
enum flags { FLAG_C, FLAG_Z, FLAG_I, FLAG_D, FLAG_B2, FLAG_B, FLAG_V, FLAG_N };

// Set a given bit (flag) of the p register
void set_flag(CPU* cpu, uint8_t flag);

// Clear a given bit (flag) of the p register
void clear_flag(CPU* cpu, uint8_t flag);

// Check if a given bit (flag) of the p register is set
bool check_flag(CPU* cpu, uint8_t flag);

// Check if Z flag needs setting and do so if required
void handle_flag_z(CPU* cpu, uint8_t val);

// Check if N flag needs setting and do so if required
void handle_flag_n(CPU* cpu, uint8_t val);

/* ----------------------------------- Stack handling ----------------------------------- */
void stack_push(CPU* cpu, uint8_t val);
uint8_t stack_pull(CPU* cpu);

/* ----------------------------------- Addressing Modes ----------------------------------- */
// Absolute
uint16_t addr_abs(CPU* cpu);

// Absolute (X indexed)
uint16_t addr_abx(CPU* cpu);

// Absolute (Y indexed)
uint16_t addr_aby(CPU* cpu);

// Accumulator
uint16_t addr_acc(CPU* cpu);

// Immediate
uint16_t addr_imm(CPU* cpu);

// Indirect
uint16_t addr_ind(CPU* cpu);

// Indirect (X indexed)
uint16_t addr_inx(CPU* cpu);

// Indirect (Y indexed)
uint16_t addr_iny(CPU* cpu);

// Relative
uint16_t addr_rel(CPU* cpu);

// Zero Page
uint8_t addr_zpg(CPU* cpu);

// Zero Page (X indexed)
uint8_t addr_zpx(CPU* cpu);

// Zero Page (Y indexed)
uint8_t addr_zpy(CPU* cpu);