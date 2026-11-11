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

int main(int argc, char* argv[]) { //! argument 1 - filename of the binary; arg 2 - address in ram where to load the program to. 0 by default
    uint16_t load_program_addr; //? Ram address to load the program in
    
    if (argc == 1) {
        printf("Arguments not provided.");
        exit(1);
    }

    else if (argc == 2) {
        printf("The address in ram to load the program chosen 0 by default.");
        load_program_addr = 0;
    }

    else if (argc == 3) {
        load_program_addr = string_to_hex(argv[2]);
    }

    FILE* file = fopen(argv[1], "rb");

    if (file == NULL) {
        printf("Could not open the file\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    rewind(file);

    uint8_t *file_buffer = calloc(fsize+2, sizeof(unsigned char));

    fread(file_buffer, sizeof(uint8_t), fsize, file);

    State8080 state;

    initializeI8080(&state);

    memcpy(&state.memory[load_program_addr], file_buffer, fsize);

    free(file_buffer);

    int i=0;
    while (i<=fsize) {
        printf("N%06d\n\nREGISTERS:\na – %04x\nb – %04x\nc – %04x\nd – %04x\ne – %04x\nh – %04x\nl – %04x\n\nFLAGS:\nz – %d\ns – %d\np – %d\nc – %d\nLocation (PC) – %04x\n", i, state.a, state.b, state.c, state.d, state.e, state.h, state.l, state.cc.z, state.cc.s, state.cc.p, state.cc.cy, state.pc);
        disassemble_i8080_instruction(state.memory, state.pc);
        //printf(" – ");
        
        emulate_i8080_instruction(&state);

        //printf("aaa \n");
        //∂printf("\n\n\n");

        i++;
    }
    
    free(state.memory);
} 



