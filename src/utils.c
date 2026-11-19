#include "utils.h"
#include "disassembler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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