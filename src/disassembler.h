#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

int disassemble_i8080_instruction(const unsigned char *codebuffer, const int pc); //? codebuffer - a pointer to the ROM; pc - program counter

#endif