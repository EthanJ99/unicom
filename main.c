#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "ops.h"
#include "system.h"

// Global containing the main system components (CPU, PPU, etc)
System nes; 

int main(int argc, char *argv[]){
    // Initialise CPU and PPU
    CPU cpu;
    cpu_init(&cpu);
    PPU ppu;
    ppu_init(&ppu);

    // Attach CPU and PPU to main system
    system_init(&cpu, &ppu);

    // Load game ROM
    char* rom_path = argv[1];
    int rom_status = load_rom(rom_path, &cpu, &ppu);

    if(rom_status == 0){
        printf("Loaded ROM from '%s'\n", rom_path);
    } else{
        if(argc < 2){
            printf("ERROR! Not enough arguments.\nUsage: unicom.exe {path_to_rom}\n");
        } else{
            printf("ERROR! Failed to load ROM from '%s'\n", rom_path);
        }
        exit(1);
    }

    // Set PC to first instruction
    cpu.pc =  cpu.memory[0xfffc] | (cpu.memory[0xfffd] << 8);

    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    // Create a window of 128x128 pixels to draw a single pattern table
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(256, 128, 0, &window, &renderer);
    // SDL_RenderSetScale(renderer, 4.0,4.0);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    draw_pattern_table(window, renderer, 0);
    draw_pattern_table(window, renderer, 0x1000);

    // // DEBUG Set PC to NESTEST start point
    // // cpu.pc = 0xc000;

    printf("PC: %x (%d)\n", cpu.pc, cpu.pc);

    bool running = true;

    // Count no. instructions executed
    int loop = 1;

    int cpu_cycles = 0;
    int ppu_cycles = 0;

    /*uint64_t start_time;
    uint64_t end_time;
    float elapsed_time;*/
    Uint32 start_time;
    Uint32 end_time;

    Uint32 delta;

    // Main execution loop
    while(running){
        start_time = SDL_GetTicks();
        delta = start_time - end_time;

        if(delta > 1000/60.0){
            end_time = start_time;
            // Handle input
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT){
                
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                break;
            }

            //printf("[%d] ", loop);
            loop++;
            
            // Main CPU/PPU Execution
            cpu_cycles = cpu_step(&cpu);
            
            ppu_cycles += cpu_cycles * 3;
            while(ppu_cycles > 0){
                ppu_step(&ppu, renderer);
                ppu_cycles--;
            }
            SDL_RenderPresent(renderer);
        }

        
        
    }

    // Print nestest results
    // printf("NESTEST Results [0x02]: %.2X [0x03]: %.2X", cpu.memory[0x02], cpu.memory[0x03]);

    return 0;
}