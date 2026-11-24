#include "../emulator.h"
#include "../utils.h"

#include "raylib.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HALF_FRAME_CYCLES 16666
#define SCALING 5
#define RESOLUTION_Y 256
#define RESOLUTION_X 224

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

void update_display(const uint8_t *memory) { //? Updates the space invaders display
    //& image is at $2400-$3fff. 256x224 pixels
    BeginDrawing();
    ClearBackground(BLACK);

    Color pixel_color;
    int actual_x;
    int actual_y;

    int x;
    for (int y=0; y<RESOLUTION_X; y++) {
        for (int x8=0; x8<RESOLUTION_Y/8; x8++) {
            for (int i=0; i<8; i++) {
                x=x8*8+i;
                if (((memory[0x2400+(224*256/8)-1-(y*(256/8)+x8)] << i >> 7) & 1) == 1) {
                    //* the screen is stored rotated in memory, so i switch x and y
                    actual_x = (RESOLUTION_X-1-y)*SCALING;
                    actual_y = x*SCALING;
                    if (actual_y>=32*SCALING && actual_y<64*SCALING) { pixel_color = RED; }
                    else if (actual_y>=184*SCALING && actual_y<240*SCALING) { pixel_color = GREEN; }
                    else if (actual_y>=240*SCALING && actual_x<136*SCALING) { pixel_color = GREEN; }
                    else { pixel_color = WHITE; }

                    DrawRectangle(actual_x, actual_y, SCALING, SCALING, pixel_color);
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
            case 1: //? Mid-screen interrupt (draws top half)
                {
                    generateInterrupt(&(state->interrupt_bus), 0xCF); //* 0xCF = RST 1
                    break;
                }
            case 0: //? End-of-frame interrupt (draws bottom half)
                {   
                    update_display(state->memory);
                    generateInterrupt(&(state->interrupt_bus), 0xD7); //* 0xD7 = RST 2
                    break;
                }
        }
    }
}

void space_invaders_shift_register_io_handler(IO_Bus *io_bus, Shift_Register *shift_register) {
    if (io_bus->did_write) {
        switch (io_bus->used_port_id) {
            case 2: //^ shift amount inout
                {
                    shift_register->offset = io_bus->io_ports[2].write_data&0x07;
                    io_bus->io_ports[3].read_data = get_shift_register_data(shift_register);
                } break;
            case 4: //^ shift register input
                {
                    write_into_shift_register(shift_register, io_bus->io_ports[4].write_data);
                    io_bus->io_ports[3].read_data = get_shift_register_data(shift_register);
                } break;
        }
        io_bus->did_write = 0;
    }
}


void space_invaders_input_handler(IO_Bus *io_bus) {
    uint8_t port_data_0 = 0x0E; //* bits 1,2,3 always 1
    uint8_t port_data_1 = 0x08; //* bit 3 always 1
    uint8_t port_data_2 = 0x00;

    if (IsKeyDown(KEY_C)) { port_data_1 |= 0x01; } //^ port 1 bit 0; Coin insert
    if (IsKeyDown(KEY_TWO)) { port_data_1 |= 0x02; } //^ port 1 bit 1; Player 2 start
    if (IsKeyDown(KEY_ONE)) { port_data_1 |= 0x04; } //^ port 1 bit 3; Player 1 start
    if (IsKeyDown(KEY_S)) { port_data_1 |= 0x10; } //^ port 1 bit 4; Player 1 shoot
    if (IsKeyDown(KEY_A)) { port_data_1 |= 0x20; } //^ port 1 bit 5; Player 1 left
    if (IsKeyDown(KEY_D)) { port_data_1 |= 0x40; } //^ port 1 bit 6; Player 1 right
    if (IsKeyDown(KEY_K)) { port_data_2 |= 0x10; } //^ port 2 bit 4; Player 2 shoot
    if (IsKeyDown(KEY_J)) { port_data_2 |= 0x20; } //^ port 2 bit 5; Player 2 left
    if (IsKeyDown(KEY_L)) { port_data_2 |= 0x40; } //^ port 2 bit 6; Player 2 right
    //& this three are named like this in the hardware breakdown, but they don't really do anything somehow
    if (IsKeyDown(KEY_SPACE)) { port_data_0 |= 0x10; } //^ port 0 bit 4; Shoot
    if (IsKeyDown(KEY_LEFT)) { port_data_0 |= 0x20; } //^ port 0 bit 5; Left
    if (IsKeyDown(KEY_RIGHT)) { port_data_0 |= 0x40; } //^ port 0 bit 6; Right
    //& ---

    io_bus->io_ports[0].read_data = port_data_0;
    io_bus->io_ports[1].read_data = port_data_1;
    io_bus->io_ports[2].read_data = port_data_2;

}


int main(int argc, char* argv[]) { //* arg 1 - filename of the binary; arg 2 - address in ram where to load the program to. 0 by default
    //! Argument handling
    
    if (argc == 1) {
        printf("Arguments not provided.");
        exit(1);
    }

    //! ---
    
    //! Window handling
    InitWindow(RESOLUTION_X * SCALING, RESOLUTION_Y * SCALING, "Space Invaders Intel 8080");
    SetTargetFPS(60); //& Limits CPU speed with FPS for now, will migrate into in-loop limiter using a timer later.
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
    //long long i=0;
    while (state->pc<=fsize && !WindowShouldClose()) {
        previous_overall_clock_cycles = overall_clock_cycles;

        cpu_emulation_handler(state, &overall_clock_cycles);
        space_invaders_video_interrupts_handler(state, overall_clock_cycles, previous_overall_clock_cycles);
        space_invaders_shift_register_io_handler(&(state->io_bus), &shift_register);
        space_invaders_input_handler(&(state->io_bus));

        //i++;
    }
    //! ---

    CloseWindow();
    free(state->memory);
} 



