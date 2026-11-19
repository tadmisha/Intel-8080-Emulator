#include "emulator.h"
#include "disassembler.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

uint16_t string_to_hex(char *string) {
    size_t string_l = strlen(string);
    if (!((string[0] == '0') && (string[1] == 'x')) || (string_l < 3) || (string_l > 6)) {
        printf("The argument provided is not a valid hexademical 16-bit address.\n");
        exit(1);
    }
    
    int idx = string_l-1;
    char sym; //? A character in the input string
    uint8_t hex_digit; //?  A variable for "sym"-s hex digitt equivalent


    uint16_t hexademical = 0;

    while (idx>1) {
        sym = string[idx];

        if (sym >= 48 && sym <= 57) {
            hex_digit = (sym-48);
        }

        else if (sym >= 65 && sym <= 70) {
            hex_digit = (sym-65)+10;
        }

        else if (sym >= 97 && sym <= 102) {
            hex_digit = (sym-97)+10;
        }

        else {
            printf("\nSymbol '%c' is not a hexademical digit.\n", sym);
            exit(1);
        }

        hexademical = hexademical | ((0x0000 | hex_digit) << 4*(string_l-idx-1));
        idx--;
    }

    return hexademical;
}

int read_binary_file(const char *file_name, uint8_t **file_buffer) { //? Reads file into file_buffer, return file size
    FILE* file = fopen(file_name, "rb");

    if (file == NULL) {
        printf("Could not open the file\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    rewind(file);

    *file_buffer = calloc(fsize+2, sizeof(unsigned char));

    fread(*file_buffer, sizeof(uint8_t), fsize, file);

    return fsize;
}


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

    int i=0;
    while (state->pc<=fsize) {
        printf("N%06d\n", i);
        emulation_handler(state);

        i++;
    }
    
    free(state->memory);
} 



