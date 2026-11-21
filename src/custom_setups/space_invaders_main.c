#include "../emulator.h"
#include "../utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void space_invaders_video_interrupts_handler(Interrupt_Bus *interrupt_bus, const unsigned long long overall_clock_cycles, const unsigned long long previous_overall_clock_cycles) { //? Handles the frame and top frame interrupts that space invaders uses to generate the image buffer.
    //& Intel 8080 is 2MHZ, space invaders runs at 60 FPS => screen needs to be refreshed every 2,000,000/60≈33333, half interrupt at 16666
    int prev_halves_passed = previous_overall_clock_cycles/16666;
    int halves_passed = overall_clock_cycles/16666;

    if (prev_halves_passed != halves_passed) {
        switch (halves_passed%2){
            case 1: generateInterrupt(interrupt_bus, 0xCF); break; //* 0xCF = RST 1
            case 0: generateInterrupt(interrupt_bus, 0xD7); break; //* 0xD7 = RST 2
        }
    }
}

void print_display(const uint8_t *memory) { //? Prints the space invaders display in the terminal
    //& image is at $2400-$3fff. 256x224 pixels
    for (int y8=0; y8<224; y8++) {
        for (int x8=0; x8<32; x8++) {
            for (int i=0; i<8; i++) {
                printf("%s", ((memory[0x2400+y8*32+x8] << i >> 7) & 1) == 1 ? "□" : "■");
            }
        }
        printf("\n");
    }
    for (int i=0; i<10; i++) {
        printf("\n");
    }
}

int main(int argc, char* argv[]) { //* arg 1 - filename of the binary; arg 2 - address in ram where to load the program to. 0 by default
    //! Argument handling
    
    if (argc == 1) {
        printf("Arguments not provided.");
        exit(1);
    }

    //! ---
    
    //! File reading 
    uint8_t *binary_file_buffer;
    int fsize = read_binary_file(argv[1], &binary_file_buffer);

    State8080 *state;
    initializeI8080(&state);

    memcpy(state->memory, binary_file_buffer, fsize); //? Loads the binary into the state memory
    free(binary_file_buffer);
    //! ---

    unsigned long long previous_overall_clock_cycles = 0;
    unsigned long long overall_clock_cycles = 0;
    int i=0;
    while (state->pc<=fsize) {
        previous_overall_clock_cycles = overall_clock_cycles;

        cpu_emulation_handler(state, &overall_clock_cycles);
        space_invaders_video_interrupts_handler(&(state->interrupt_bus), overall_clock_cycles, previous_overall_clock_cycles);
        //print_cpu_state(state, i);

        i++;

        /*if (i>4626000) {
            print_cpu_state(state, i);
        }*/

        if (i%1000000==0) {
            print_display(state->memory);
        }
    }
    
    free(state->memory);
} 



