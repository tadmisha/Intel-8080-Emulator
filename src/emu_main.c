#include "emulator.h"
#include "disassembler.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "r");

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

    memcpy(state.memory, file_buffer, fsize);

    free(file_buffer);

    int i=0;
    while (1) {
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



