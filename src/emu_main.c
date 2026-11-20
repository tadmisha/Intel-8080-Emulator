#include "emulator.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) { //* arg 1 - filename of the binary; arg 2 - address in ram where to load the program to. 0 by default
    //! Argument handling
    uint16_t load_program_addr; //? Ram address to load the program in
    
    if (argc == 1) {
        printf("Arguments not provided.");
        exit(1);
    }
    else if (argc == 2) {
        printf("The address in ram to load the program chosen 0 by default.\n\n\n\n\n");
        load_program_addr = 0;
    }
    else if (argc == 3) {
        load_program_addr = string_to_hex(argv[2]);
    }
    //! ---
    
    //! File reading 
    uint8_t *binary_file_buffer;
    int fsize = read_binary_file(argv[1], &binary_file_buffer);

    State8080 *state;
    initializeI8080(&state);

    memcpy(state->memory+load_program_addr, binary_file_buffer, fsize); //? Loads the binary into the state memory
    free(binary_file_buffer);
    //! ---

    long long overall_clock_cycles = 0;
    int i=0;
    while (state->pc<=fsize) {
        cpu_emulation_handler(state, &overall_clock_cycles);
        print_cpu_state(state, i);

        i++;
    }
    
    free(state->memory);
} 



