#pragma once

#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "ppu.h"

int load_rom(char* path, CPU* cpu, PPU* ppu);