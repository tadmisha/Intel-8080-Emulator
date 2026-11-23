#include "../emulator.h"
#include "../utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void space_invaders_input_handler(IO_Bus *io_bus, Shift_Register *shift_register) {
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

void print_display(const uint8_t *memory) { //? Prints the space invaders display in the terminal
    //& image is at $2400-$3fff. 256x224 pixels
    for (int y8=0; y8<224; y8++) {
        for (int x8=0; x8<32; x8++) {
            for (int i=0; i<8; i++) {
                printf("%s ", ((memory[0x2400+y8*32+x8] << i >> 7) & 1) == 1 ? "□" : "■");
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

    Shift_Register shift_register;
    initialize_shift_register(&shift_register);

    unsigned long long previous_overall_clock_cycles = 0;
    unsigned long long overall_clock_cycles = 0;

    //! Main loop
    int i=0;
    while (state->pc<=fsize) {
        previous_overall_clock_cycles = overall_clock_cycles;

        cpu_emulation_handler(state, &overall_clock_cycles);
        space_invaders_video_interrupts_handler(&(state->interrupt_bus), overall_clock_cycles, previous_overall_clock_cycles);
        space_invaders_input_handler(&(state->io_bus), &shift_register);

        if (i%100000==0) {
            print_display(state->memory);
        }

        i++;
    }
    //! ---
    
    free(state->memory);
} 



