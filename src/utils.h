#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include "emulator.h"
#include "disassembler.h"

uint16_t string_to_hex(char *string); //? Converts a string hex into hex ("0x12F3" -> 0x12F3)

int read_binary_file(const char *file_name, uint8_t **file_buffer); //? Reads file into file_buffer, return file size

static inline void print_cpu_state(const State8080 *state, int instruction_number) { //? Print information on the CPU (registers, etc)
    printf("Instruction N%010d\nREGISTERS:\na – %04x\nb – %04x\nc – %04x\nd – %04x\ne – %04x\nh – %04x\nl – %04x\n\nFLAGS:\nz – %d\ns – %d\np – %d\nc – %d\nLocation (PC) – %04x\n", instruction_number, state->a, state->b, state->c, state->d, state->e, state->h, state->l, state->cc.z, state->cc.s, state->cc.p, state->cc.cy, state->pc);
    disassemble_i8080_instruction(state->memory, state->pc);
}

#endif