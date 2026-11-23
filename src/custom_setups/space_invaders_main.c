#include "../emulator.h"
#include "../utils.h"

#include "raylib.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HALF_FRAME_CYCLES 16666
#define SCALING 3
#define RESOLUTION_X 256
#define RESOLUTION_Y 224

//! Shift register implementation
typedef struct Shift_Register {
    uint16_t data;
    uint8_t offset;
} Shift_Register;

void initialize_shift_register(Shift_Register *shift_register) {
    shift_register->data = 0x0000;
    shift_register->offset = 0;
}

static inline uint8_t get_shift_register_data(const Shift_Register *shift_register) {
    return (uint8_t) (shift_register->data << shift_register->offset >> 8);
}

static inline void write_into_shift_register(Shift_Register *shift_register, uint8_t input_data) {
    shift_register->data = (shift_register->data >> 8) | ((uint16_t) input_data << 8);
}

//! ---
void print_display(const uint8_t *memory) { //? Prints the space invaders display in the terminal
    //& image is at $2400-$3fff. 256x224 pixels
    BeginDrawing();
    ClearBackground(BLACK);
    int x;
    for (int y=0; y<224; y++) {
        for (int x8=0; x8<32; x8++) {
            for (int i=0; i<8; i++) {
                x=x8*8+i;
                if (((memory[0x2400+(224*256/8)-(y*(256/8)+x8)] << i >> 7) & 1) == 1) {
                    DrawRectangle(x*SCALING, y*SCALING, SCALING, SCALING, WHITE);
                }
            }
        }
    }
    EndDrawing();
}

void space_invaders_video_interrupts_handler(State8080 *state, const unsigned long long overall_clock_cycles, const unsigned long long previous_overall_clock_cycles) { //? Handles the frame and top frame interrupts that space invaders uses to generate the image buffer.
    //& Intel 8080 is 2MHZ, space invaders runs at 60 FPS => screen needs to be refreshed every 2,000,000/60≈33333, half interrupt at 16666
    int prev_halves_passed = previous_overall_clock_cycles/HALF_FRAME_CYCLES;
    int halves_passed = overall_clock_cycles/HALF_FRAME_CYCLES;

    if (prev_halves_passed != halves_passed) {
        switch (halves_passed%2){
            case 1: 
                {
                    generateInterrupt(&(state->interrupt_bus), 0xCF); //* 0xCF = RST 1
                    print_display(state->memory);
                    break;
                }
            case 0:
                {
                    generateInterrupt(&(state->interrupt_bus), 0xD7); //* 0xD7 = RST 2
                    break;
                }
        }
    }
}

void space_invaders_io_handler(IO_Bus *io_bus, Shift_Register *shift_register) {
    if (io_bus->did_write) {
        switch (io_bus->used_port_id) {
            case 02: //^ shift amount inout
                {
                    shift_register->offset = io_bus->io_ports[2].write_data;
                    io_bus->io_ports[3].read_data = get_shift_register_data(shift_register);
                } break;
            case 04: //^ shift register input
                {
                    write_into_shift_register(shift_register, io_bus->io_ports[4].write_data);
                    io_bus->io_ports[3].read_data = get_shift_register_data(shift_register);
                } break;
        }
        io_bus->did_write = 0;
    }
}

int main(int argc, char* argv[]) { //* arg 1 - filename of the binary; arg 2 - address in ram where to load the program to. 0 by default
    //! Argument handling
    
    if (argc == 1) {
        printf("Arguments not provided.");
        exit(1);
    }

    //! ---
    
    //! Window handling
    InitWindow(RESOLUTION_Y * SCALING, RESOLUTION_X * SCALING, "Space Invaders Intel 8080");
    SetTargetFPS(60);

    //! File reading 
    uint8_t *binary_file_buffer;
    int fsize = read_binary_file(argv[1], &binary_file_buffer);

    State8080 *state;
    initializeI8080(&state);

    memcpy(state->memory, binary_file_buffer, fsize); //? Loads the binary into the state memory
    free(binary_file_buffer);
    //! ---

    Shift_Register shift_register;
    initialize_shift_register(&shift_register);

    unsigned long long previous_overall_clock_cycles = 0;
    unsigned long long overall_clock_cycles = 0;

    //! Main loop
    int i=0;
    while (state->pc<=fsize) {
        previous_overall_clock_cycles = overall_clock_cycles;

        cpu_emulation_handler(state, &overall_clock_cycles);
        space_invaders_video_interrupts_handler(state, overall_clock_cycles, previous_overall_clock_cycles);
        space_invaders_io_handler(&(state->io_bus), &shift_register);

        i++;
    }
    //! ---
    
    free(state->memory);
} 



