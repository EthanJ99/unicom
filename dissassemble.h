#pragma once

#include <stdint.h>
#include <stdio.h>
#include "cpu.h"

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

Op* get_op_data(uint8_t opcode);
void print_dissassembly(CPU* );