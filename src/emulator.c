#include "emulator.h"
#include "disassembler.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void initializeI8080(State8080 **state_ptr) {
    *state_ptr = malloc(sizeof(State8080));

    State8080 *state = *state_ptr;

    state->memory = calloc(64*1024+1, sizeof(uint8_t)); //* 64KB memory + 1 additional byte to store interrupt operand when working with it, the additional byte is not a part of memory, used for emulation purposes

    state->a = 0x00;
    state->b = 0x00;
    state->c = 0x00;
    state->d = 0x00;
    state->e = 0x00;
    state->h = 0x00;
    state->l = 0x00;

    state->pc = 0x0000;
    state->sp = 0xEFFF; //* Unedfined on startup on a real chip, so I take the middle ground because 0 or FFFF can cause errored when sp pushed/popped in a bad binary

    state->cc.z = 0;
    state->cc.s = 0;
    state->cc.p = 0;
    state->cc.cy = 0;
    state->cc.ac = 0;

    state->interrupt_bus.interrupt_enabled = 0;
    state->interrupt_bus.did_interrupt = 0;
    state->interrupt_bus.interrupt_operand = 0x00;

    state->cpu_haulted = 0;

    memset(&state->io_bus, 0, sizeof(state->io_bus));
}

void unimplementedInstruction(const unsigned char instr) {
    printf("Error: Unimplemented instruction: %02x\n", instr);
    exit(1);
}

void emulate_i8080_instruction(State8080* state) {

    uint8_t *instr_addr;
    instr_addr = state->memory + state->pc;

    if (state->cpu_haulted && !(state->interrupt_bus.did_interrupt && state->interrupt_bus.interrupt_enabled)) { //* Interrupt is the only way to unhault a running CPU
        return;
    }

    if (state->interrupt_bus.did_interrupt && state->interrupt_bus.interrupt_enabled) {
        state->memory[0x10000] = state->interrupt_bus.interrupt_operand;
        instr_addr = state->memory + 0x10000; //* 0x10000 is an additional byte specifically for interrupts and is not the part of the actual memory (which is 64KB)
        //* There are no issues with operands trying to be fetched but failing, because interrupt-provided instructions are 1 byte (do not have operands)

        state->pc--;
        state->interrupt_bus.did_interrupt = 0;
        state->interrupt_bus.interrupt_enabled = 0;

        state->cpu_haulted = 0; //* Interrupt is the only way to unhault a running CPU
    }

    switch (*instr_addr) {
        case 0x00: break; //^ NOP
        case 0x01: //^ LXI B,D16
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                state->b=op2;
                state->c=op1; 
                state->pc+=2;
            } break;
        case 0x02: //^ STAX B
            {
                state->memory[combine_two_bytes(state->b, state->c)] = state->a;
            } break;
        case 0x03: //^INX B 
            {
                put_16bit_in_2_bytes(&(state->b), &(state->c), combine_two_bytes(state->b, state->c)+1);
            } break;
        case 0x04: //^ INR B
            {
                uint16_t result = state->b+1;
                setZSP_Flags(&(state->cc), result);

                state->b=result;
            } break;
        case 0x05: //^ DCR B
            {
                uint16_t result = state->b-1;
                setZSP_Flags(&(state->cc), result);

                state->b=result;;
            } break;
        case 0x06: //^ MVI B, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->b=op;
                state->pc++;
            } break;
        case 0x07: //^ RLC
            {
                state->cc.cy = state->a>>7;
                state->a = state->a<<1 | state->cc.cy;
            } break;
        case 0x08: //^ -
            break;
        case 0x09: //^ DAD B
            {
                uint32_t result = combine_two_bytes(state->h, state->l)+combine_two_bytes(state->b, state->c);
                put_16bit_in_2_bytes(&(state->h), &(state->l), result);
                state->cc.cy = (result >> 16) & 1;
            } break;
        case 0x0A: //^ LDAX B
            {
                state->a = state->memory[combine_two_bytes(state->b, state->c)];
            } break;
        case 0x0B: //^ DCX B
            {
                put_16bit_in_2_bytes(&(state->b), &(state->c), combine_two_bytes(state->b, state->c)-1);
            } break;
        case 0x0C: //^ INR C
            {
                uint16_t result = state->c+1;
                setZSP_Flags(&(state->cc), result);

                state->c=result;
            } break;
        case 0x0D: //^ DCR C
            {
                uint16_t result = state->c-1;
                setZSP_Flags(&(state->cc), result);

                state->c=result;
            } break;
        case 0x0E: //^ MVI C,D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->c=op;
                state->pc++;
            } break;
        case 0x0F: //^ RRC
            {
                state->cc.cy = state->a&1;
                uint8_t result = (state->a >> 1) | (state->a << 7);

                state->a = result;
            } break;
        case 0x10: //^ -
            break;
        case 0x11: //^ LXI D,D16
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                state->d=op2;
                state->e=op1; 
                state->pc+=2;
            } break;
        case 0x12: //^ STAX D
            {
                state->memory[combine_two_bytes(state->d, state->e)] = state->a;
            } break;
        case 0x13: //^INX D
            {
                put_16bit_in_2_bytes(&(state->d), &(state->e), combine_two_bytes(state->d, state->e)+1);
            } break;
        case 0x14: //^INR D
            {
                uint16_t result = state->d+1;
                setZSP_Flags(&(state->cc), result);

                state->d=result;
            } break;
        case 0x15: //^DCR D 
            {
                uint16_t result = state->d-1;
                setZSP_Flags(&(state->cc), result);

                state->d=result;;
            } break;
        case 0x16://^ MVI D, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->d=op;
                state->pc++;
            } break;
        case 0x17: //^ RAL
            {   
                uint8_t temp = state->cc.cy;
                state->cc.cy = state->a>>7;
                state->a = (state->a << 1) | temp;
            } break;
        case 0x18: //^ -
            {
                break;
            }
        case 0x19: //^ DAD D
            {
                uint32_t result = combine_two_bytes(state->h, state->l)+combine_two_bytes(state->d, state->e);
                put_16bit_in_2_bytes(&(state->h), &(state->l), result);
                state->cc.cy = (result >> 16) & 1;
            } break;
        case 0x1A: //^ LDAX D
            {
                state->a = state->memory[combine_two_bytes(state->d, state->e)];
            } break;
        case 0x1B: //^ DCX D
            {
                put_16bit_in_2_bytes(&(state->d), &(state->e), combine_two_bytes(state->d, state->e)-1);
            } break;
        case 0x1C: //^ INR E
            {
                uint16_t result = state->e+1;
                setZSP_Flags(&(state->cc), result);

                state->e=result;
            } break;
        case 0x1D: //^ DCR E
            {
                uint16_t result = state->e-1;
                setZSP_Flags(&(state->cc), result);

                state->e=result;;
            } break;
        case 0x1E: //^ MVI E, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->e=op;
                state->pc++;
            } break;
        case 0x1F: //^ RAR 
            {   
                uint8_t temp = state->cc.cy;
                state->cc.cy = state->a&1;
                uint8_t result = (state->a >> 1) | (temp<<7);

                state->a = result;
            } break;
        case 0x20: //^ -
            {
                break;
            }
        case 0x21: //^ LXI H,D16
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                state->h=op2;
                state->l=op1; 
                state->pc+=2;
            } break;
        case 0x22: //^ SHLD adr
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                uint16_t addr = combine_two_bytes(op2, op1); 
                state->memory[addr] = state->l;
                state->memory[addr+1] = state->h;
                state->pc+=2;
            } break;
        case 0x23: //^ INX H
            {
                put_16bit_in_2_bytes(&(state->h), &(state->l), combine_two_bytes(state->h, state->l)+1);
            } break;
        case 0x24: //^ INR H
            {
                uint16_t result = state->h+1;
                setZSP_Flags(&(state->cc), result);

                state->h=result;
            } break;
        case 0x25: //^ DCR H
            {
                uint16_t result = state->h-1;
                setZSP_Flags(&(state->cc), result);

                state->h=result;;
            } break;
        case 0x26: //^ MVI H, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->h=op;
                state->pc++;
            } break;
        case 0x27: //^ DAA
            {
                uint8_t lower_4bits = (state->a) & 0x0F;
                uint8_t higher_4bits = (state->a)>>4;
                
                uint8_t correction = 0;
                if (lower_4bits>9 || state->cc.ac) {
                    correction += 0x06;
                }

                if (higher_4bits>9 || state->cc.cy) {
                    correction += 0x60;
                }

                uint16_t result = state->a+correction;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result >> 8) != 0;
                //! Imagine AC is implemented here

                state->a = result & 0xFF;
            } break;
        case 0x28: //^ -
            {
                break;
            }
        case 0x29: //^ DAD H
            {
                uint32_t result = combine_two_bytes(state->h, state->l)+combine_two_bytes(state->h, state->l);
                put_16bit_in_2_bytes(&(state->h), &(state->l), result);
                state->cc.cy = (result >> 16) & 1;
            } break;
        case 0x2A: //^ LHLD adr
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                uint16_t addr = combine_two_bytes(op2, op1); 
                state->l = state->memory[addr];
                state->h = state->memory[addr+1];
                state->pc+=2;
            } break;
        case 0x2B: //^ DCX H
            {
                put_16bit_in_2_bytes(&(state->h), &(state->l), combine_two_bytes(state->h, state->l)-1);
            } break;
        case 0x2C: //^ INR L
            {
                uint16_t result = state->l+1;
                setZSP_Flags(&(state->cc), result);

                state->l=result;
            } break;
        case 0x2D: //^ DCR L
            {
                uint16_t result = state->l-1;
                setZSP_Flags(&(state->cc), result);

                state->l=result;;
            } break;
        case 0x2E: //^ MVI L, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->l=op;
                state->pc++;
            } break;
        case 0x2F: //^ CMA
            {
                state->a = ~state->a;
            } break;
        case 0x30: //^ -
            {
                break;
            }
        case 0x31: //^ LXI SP,D16
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                state->sp = combine_two_bytes(op2, op1);
                state->pc+=2;
            } break;
        case 0x32: //^ STA adr
            {
                uint8_t op2=get_operand(instr_addr, 2);
                uint8_t op1=get_operand(instr_addr, 1);
                state->memory[combine_two_bytes(op2, op1)] = state->a;
                state->pc+=2;
            } break;
        case 0x33: //^ INX SP
            {
                state->sp++;
            } break;
        case 0x34: //^ INR M
            {
                uint16_t result = state->memory[combine_two_bytes(state->h, state->l)] + 1;
                setZSP_Flags(&(state->cc), result);

                state->memory[combine_two_bytes(state->h, state->l)] = result;
                
            } break;
        case 0x35: //^ DCR M
            {
                uint16_t result = state->memory[combine_two_bytes(state->h, state->l)] - 1;
                setZSP_Flags(&(state->cc), result);

                state->memory[combine_two_bytes(state->h, state->l)] = result;
            } break;
        case 0x36: //^ MVI M, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->memory[combine_two_bytes(state->h, state->l)]=op;
                state->pc++;
            } break;
        case 0x37: //^ STC
            {
                state->cc.cy = 1;
            } break;
        case 0x38: //^ -
            {
                break;
            }
        case 0x39: //^ DAD SP
            {
                uint32_t result = combine_two_bytes(state->h, state->l)+state->sp;
                put_16bit_in_2_bytes(&(state->h), &(state->l), result);
                state->cc.cy = (result >> 16) & 1;
            } break;
        case 0x3A: //^ LDA adr
            {
                int8_t op1 = get_operand(instr_addr, 1);
                int8_t op2 = get_operand(instr_addr, 2);

                state->a = state->memory[combine_two_bytes(op2, op1)];

                state->pc+=2;
            } break;
        case 0x3B: //^ DCX SP
            {
                state->sp--;
            } break;
        case 0x3C: //^ INR A
            {
                uint16_t result = state->a+1;
                setZSP_Flags(&(state->cc), result);

                state->a=result;
            } break;
        case 0x3D: //^ DCR A
            {
                uint16_t result = state->a-1;
                setZSP_Flags(&(state->cc), result);

                state->a=result;;
            } break;
        case 0x3E: //^ MVI A, D8
            {
                uint8_t op=get_operand(instr_addr, 1);
                state->a=op;
                state->pc++;
            } break;
        case 0x3F: //^ CMC
            {
                state->cc.cy = !state->cc.cy;
            } break;
        case 0x40: //^ MOV B,B
            {
                state->b = state->b;
            } break;
        case 0x41: //^ MOV B,C
            {
                state->b = state->c;
            } break;
        case 0x42: //^ MOV B,D
            {
                state->b = state->d;
            } break;
        case 0x43: //^ MOV B,E
            {
                state->b = state->e;
            } break;
        case 0x44: //^ MOV B,H
            {
                state->b = state->h;
            } break;
        case 0x45://^ MOV B,L
            {
                state->b = state->l;
            } break;
        case 0x46: //^ MOV B,M
            {

                state->b = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x47: //^ MOV B,A
            {
                state->b = state->a;
            } break;
        case 0x48: //^ MOV C,B
            {
                state->c = state->b;
            } break;
        case 0x49: //^ MOV C,C
            {
                state->c = state->c;
            } break;
        case 0x4A: //^ MOV C,D
            {
                state->c = state->d;
            } break;
        case 0x4B: //^ MOV C,E
            {
                state->c = state->e;
            } break;
        case 0x4C: //^ MOV C,H
            {
                state->c = state->h;
            } break;
        case 0x4D: //^ MOV C,L
            {
                state->c = state->l;
            } break;
        case 0x4E: //^ MOV C,M
            {
                state->c = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x4F: //^ MOV C,A
            {
                state->c = state->a;
            } break;
        case 0x50: //^ MOV D,B
            {
                state->d = state->b;
            } break;
        case 0x51: //^ MOV D,C
            {
                state->d = state->c;
            } break;
        case 0x52: //^ MOV D,D
            {
                state->d = state->d;
            } break;
        case 0x53: //^ MOV D,E
            {
                state->d = state->e;
            } break;
        case 0x54: //^ MOV D,H
            {
                state->d = state->h;
            } break;
        case 0x55: //^ MOV D,L
            {
                state->d = state->l;
            } break;
        case 0x56: //^ MOV D,M
            {
                state->d = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x57: //^ MOV D,A
            {
                state->d = state->a;
            } break;
        case 0x58: //^ MOV E,B
            {
                state->e = state->b;
            } break;
        case 0x59: //^ MOV E,C
            {
                state->e = state->c;
            } break;
        case 0x5A: //^ MOV E,D
            {
                state->e = state->d;
            } break;
        case 0x5B: //^ MOV E,E
            {
                state->e = state->e;
            } break;
        case 0x5C: //^ MOV E,H
            {
                state->e = state->h;
            } break;
        case 0x5D: //^ MOV E,L
            {
                state->e = state->l;
            } break;
        case 0x5E: //^ MOV E,M
            {
                state->e = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x5F: //^ MOV E,A
            {
                state->e = state->a;
            } break;
        case 0x60: //^ MOV H,B
            {
                state->h = state->b;
            } break;
        case 0x61: //^ MOV H,C
            {
                state->h = state->c;
            } break;
        case 0x62://^ MOV H,D
            {
                state->h = state->d;
            } break;
        case 0x63: //^ MOV H,E
            {
                state->h = state->e;
            } break;
        case 0x64://^ MOV H,H
            {
                state->h = state->h;
            } break;
        case 0x65://^ MOV H,L
            {
                state->h = state->l;
            } break;
        case 0x66://^ MOV H,M
            {
                state->h = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x67: //^ MOV H,A
            {
                state->h = state->a;
            } break;
        case 0x68: //^ MOV L,B
            {
                state->l = state->b;
            } break;
        case 0x69: //^ MOV L,C
            {
                state->l = state->c;
            } break;
        case 0x6A: //^ MOV L,D
            {
                state->l = state->d;
            } break;
        case 0x6B: //^ MOV L,E
            {
                state->l = state->e;
            } break;
        case 0x6C: //^ MOV L,H
            {
                state->l = state->h;
            } break;
        case 0x6D: //^ MOV L,L
            {
                state->l = state->l;
            } break;
        case 0x6E: //^ MOV L,M
            {
                state->l = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x6F: //^ MOV L,A
            {
                state->l = state->a;
            } break;
        case 0x70: //^ MOV M,B 
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->b;
            } break;
        case 0x71: //^ MOV M,C
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->c;
            } break;
        case 0x72: //^ MOV M,D
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->d;
            } break;
        case 0x73: //^ MOV M,E
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->e;
            } break;
        case 0x74: //^ MOV M,H
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->h;
            } break;
        case 0x75: //^ MOV M,L
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->l;
            } break;
        case 0x76: //^ HLT
            {
                state->cpu_haulted=1;
            } break;
        case 0x77: //^ MOV M,A
            {
                state->memory[combine_two_bytes(state->h, state->l)] = state->a;
            } break;
        case 0x78: //^ MOV A,B
            {
                state->a = state->b;
            } break;
        case 0x79: //^ MOV A,C
            {
                state->a = state->c;
            } break;
        case 0x7A: //^ MOV A,D
            {
                state->a = state->d;
            } break;
        case 0x7B: //^ MOV A,E
            {
                state->a = state->e;
            } break;
        case 0x7C: //^ MOV A,H
            {
                state->a = state->h;
            } break;
        case 0x7D: //^ MOV A,L
            {
                state->a = state->l;
            } break;
        case 0x7E: //^ MOV A,M
            {
                state->a = state->memory[combine_two_bytes(state->h, state->l)];
            } break;
        case 0x7F: //^ MOV A,A
            {
                state->a = state->a;
            } break;
        case 0x80: //^ ADD B
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->b;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x81: //^ ADD C
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->c;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x82: //^ ADD D
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->d;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x83: //^ ADD E
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->e;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x84: //^ ADD H
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->h;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x85: //^ ADD L
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->l;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x86: //^ ADD M
            {
                uint16_t offset = combine_two_bytes(state->h, state->l);
                uint16_t result = (uint16_t) state->a + state->memory[offset];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x87: //^ ADD A
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->a;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x88: //^ ADC B
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->b + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x89: //^ ADC C
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->c + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x8A: //^ ADC D
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->d + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x8B: //^ ADC E
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->e + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x8C: //^ ADC H
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->h + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x8D: //^ ADC L
            {
                uint16_t result = (uint16_t) state->a + (uint16_t) state->l + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x8E: //^ ADC M
            {
                uint16_t result = (uint16_t) state->a + state->memory[combine_two_bytes(state->h, state->l)] + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x8F: //^ ADC A
            {
                uint16_t result = (uint16_t) state->a + state->a + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x90: //^ SUB B
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->b;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x91: //^ SUB C
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->c;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x92: //^ SUB D
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->d;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x93: //^ SUB E
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->e;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x94: //^ SUB H
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->h;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x95: //^ SUB L
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->l;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x96: //^ SUB M
            {
                uint16_t result = (uint16_t) state->a - state->memory[combine_two_bytes(state->h, state->l)];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x97: //^ SUB A
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->a;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x98: //^ SBB B
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->b - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x99: //^ SBB C
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->c - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x9A: //^ SBB D
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->d - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x9B: //^ SBB E
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->e - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x9C: //^ SBB H
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->h - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x9D: //^ SBB L
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->l - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x9E: //^ SBB M
            {
                uint16_t result = (uint16_t) state->a - state->memory[combine_two_bytes(state->h, state->l)] - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0x9F: //^ SBB A
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->a - state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;
            } break;
        case 0xA0: //^ ANA B
            {
                uint16_t result = state->a & state->b;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA1: //^ ANA C
            {
                uint16_t result = state->a & state->c;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA2: //^ ANA D
            {
                uint16_t result = state->a & state->d;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA3: //^ ANA E
            {
                uint16_t result = state->a & state->e;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA4: //^ ANA H
            {
                uint16_t result = state->a & state->h;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA5: //^ ANA L
            {
                uint16_t result = state->a & state->l;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA6: //^ ANA M
            {
                uint16_t result = state->a & state->memory[combine_two_bytes(state->h, state->l)];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA7: //^ ANA A
            {
                uint16_t result = state->a & state->a;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;
            } break;
        case 0xA8: //^ XRA B
            {
                uint16_t result = state->a ^ state->b;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xA9: //^ XRA C
            {
                uint16_t result = state->a ^ state->c;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xAA: //^ XRA D
            {
                uint16_t result = state->a ^ state->d;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xAB: //^ XRA E
            {
                uint16_t result = state->a ^ state->e;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xAC: //^ XRA H
            {
                uint16_t result = state->a ^ state->h;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xAD: //^ XRA L
            {
                uint16_t result = state->a ^ state->l;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xAE: //^ XRA M
            {
                uint16_t result = state->a ^ state->memory[combine_two_bytes(state->h, state->l)];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xAF: //^ XRA A
            {
                uint16_t result = state->a ^ state->a;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB0: //^ ORA B
            {
                uint16_t result = state->a | state->b;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB1: //^ ORA C
            {
                uint16_t result = state->a | state->c;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB2: //^ ORA D
            {
                uint16_t result = state->a | state->d;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB3: //^ ORA E
            {
                uint16_t result = state->a | state->e;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB4: //^ ORA H
            {
                uint16_t result = state->a | state->h;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB5: //^ ORA L
            {
                uint16_t result = state->a | state->l;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB6: //^ ORA M
            {
                uint16_t result = state->a | state->memory[combine_two_bytes(state->h, state->l)];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB7: //^ ORA A
            {
                uint16_t result = state->a | state->a;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;                
            } break;
        case 0xB8: //^ CMP B
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->b;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xB9: //^ CMP C
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->c;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xBA: //^ CMP D
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->d;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xBB: //^ CMP E
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->e;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xBC: //^ CMP H
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->h;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xBD: //^ CMP L
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->l;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xBE: //^ CMP M
            {
                uint16_t result = (uint16_t) state->a - state->memory[combine_two_bytes(state->h, state->l)];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xBF: //^ CMP A
            {
                uint16_t result = (uint16_t) state->a - (uint16_t) state->a;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;
            } break;
        case 0xC0: //^ RNZ
            {
                if (!(state->cc.z)) {
                    uint8_t *pc_l = ((uint8_t *) &(state->pc));
                    uint8_t *pc_h = pc_l+1;

                    pop_2bytes_from_stack(state->memory, &(state->sp), pc_h, pc_l);
                }
            } break;
        case 0xC1: //^ POP B
            {
                pop_2bytes_from_stack(state->memory, &(state->sp), &(state->b), &(state->c));
            } break;
        case 0xC2: //^ JNZ
            { 
                if (state->cc.z == 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xC3: //^ JMP adr
            {
                uint8_t op1 = get_operand(instr_addr, 1);
                uint8_t op2 = get_operand(instr_addr, 2);
                uint16_t op_combined = combine_two_bytes(op2, op1);
                state->pc = op_combined;
                state->pc--;
            } break;
        case 0xC4: unimplementedInstruction(*instr_addr); break;
        case 0xC5: //^ PUSH B
            {
                push_16bit_to_stack(state->memory, &(state->sp), combine_two_bytes(state->b, state->c));
            } break;
        case 0xC6: //^ ADI
            {
                uint8_t op = get_operand(instr_addr, 1);
                uint16_t result = (uint16_t) state->a + op;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;

                state->pc++;
            } break;
        case 0xC7: unimplementedInstruction(*instr_addr); break;
        case 0xC8: //^ RZ
            {
                if (state->cc.z) {
                    uint8_t *pc_l = ((uint8_t *) &(state->pc));
                    uint8_t *pc_h = pc_l+1;

                    pop_2bytes_from_stack(state->memory, &(state->sp), pc_h, pc_l);
                }
            } break;
        case 0xC9: //^ RET
            {
                uint8_t *pc_l = ((uint8_t *) &(state->pc));
                uint8_t *pc_h = pc_l+1;

                pop_2bytes_from_stack(state->memory, &(state->sp), pc_h, pc_l);
            } break;
        case 0xCA: //^ JZ
            { 
                if (state->cc.z == 1) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xCB: unimplementedInstruction(*instr_addr); break;
        case 0xCC: unimplementedInstruction(*instr_addr); break;
        case 0xCD: //^ CALL
            {
                uint16_t return_addr = state->pc+2;
                push_16bit_to_stack(state->memory, &(state->sp), return_addr);

                uint8_t op1 = get_operand(instr_addr, 1);
                uint8_t op2 = get_operand(instr_addr, 2);
                state->pc = combine_two_bytes(op2, op1);
                state->pc--;
            } break;
        case 0xCE: //^ ACI 
            {   
                uint8_t op = get_operand(instr_addr, 1);
                
                uint16_t result =  state->a + op + state->cc.cy;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a = result;

                state->pc++;
            } break;
        case 0xCF: unimplementedInstruction(*instr_addr); break;
        case 0xD0: //^ RNC
            {
                if (!(state->cc.cy)) {
                    uint8_t *pc_l = ((uint8_t *) &(state->pc));
                    uint8_t *pc_h = pc_l+1;

                    pop_2bytes_from_stack(state->memory, &(state->sp), pc_h, pc_l);
                }
            } break;
        case 0xD1: //^ POP D
            {
                pop_2bytes_from_stack(state->memory, &(state->sp), &(state->d), &(state->e));
            } break;
        case 0xD2: //^ JNC
            {
                if (state->cc.cy == 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xD3: //^ OUT
            {   
                uint8_t op = get_operand(instr_addr, 1);
                state->io_bus.io_ports[op].inputed_data = state->a;

                state->pc++;
            } break;
        case 0xD4: unimplementedInstruction(*instr_addr); break;
        case 0xD5: //^ PUSH D
            {
                push_16bit_to_stack(state->memory, &(state->sp), combine_two_bytes(state->d, state->e));
            } break;
        case 0xD6: //^ SUI 
            {
                uint8_t op = get_operand(instr_addr, 1);
                
                uint16_t result = state->a - op;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = ((result & 0xFF00) >> 8) != 0;

                state->a = result;

                state->pc++;
            } break;
        case 0xD7: unimplementedInstruction(*instr_addr); break;
        case 0xD8: //^ RC
            {
                if (state->cc.cy) {
                    uint8_t *pc_l = ((uint8_t *) &(state->pc));
                    uint8_t *pc_h = pc_l+1;
                pop_2bytes_from_stack(state->memory, &(state->sp), pc_h, pc_l);
                }
            } break;
        case 0xD9: unimplementedInstruction(*instr_addr); break;
        case 0xDA: //^ JC
            {
                if (state->cc.cy != 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xDB: //^ IN d8
            {
                uint8_t op = get_operand(instr_addr, 1);
                state->a = state->io_bus.io_ports[op].outputed_data;

                state->pc++;
            } break;
        case 0xDC: unimplementedInstruction(*instr_addr); break;
        case 0xDD: unimplementedInstruction(*instr_addr); break;
        case 0xDE: unimplementedInstruction(*instr_addr); break;
        case 0xDF: unimplementedInstruction(*instr_addr); break;
        case 0xE0: unimplementedInstruction(*instr_addr); break;
        case 0xE1: //^ POP H
            {
                pop_2bytes_from_stack(state->memory, &(state->sp), &(state->h), &(state->l));
            } break;
        case 0xE2: //^ JPO
            {
                if (state->cc.p == 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xE3: //^ XTHL
            {   
                uint8_t intermediate;

                intermediate = state->memory[state->sp];
                state->memory[state->sp] = state->l;
                state->l = intermediate;

                intermediate = state->memory[state->sp+1];
                state->memory[state->sp+1] = state->h;
                state->h = intermediate;
            } break;
        case 0xE4: unimplementedInstruction(*instr_addr); break;
        case 0xE5: //^ PUSH H
            {
                push_16bit_to_stack(state->memory, &(state->sp), combine_two_bytes(state->h, state->l));
            } break;
        case 0xE6: //^ ANI
            {
                uint8_t op = get_operand(instr_addr, 1);
                uint16_t result = state->a & op;

                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a=result;
                
                state->pc++;

            } break;
        case 0xE7: unimplementedInstruction(*instr_addr); break;
        case 0xE8: unimplementedInstruction(*instr_addr); break;
        case 0xE9: unimplementedInstruction(*instr_addr); break;
        case 0xEA: //^ JPE
            {
                if (state->cc.p != 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xEB: //^ XCHG
            {
                uint8_t temp = state->h;
                state->h = state->d;
                state->d = temp;

                temp = state->l;
                state->l = state->e;
                state->e = temp;
            } break;
        case 0xEC: unimplementedInstruction(*instr_addr); break;
        case 0xED: unimplementedInstruction(*instr_addr); break;
        case 0xEE: unimplementedInstruction(*instr_addr); break;
        case 0xEF: unimplementedInstruction(*instr_addr); break;
        case 0xF0: unimplementedInstruction(*instr_addr); break;
        case 0xF1: //^ POP PSW
            {
                uint8_t flags_layout;

                pop_2bytes_from_stack(state->memory, &(state->sp), &(state->a), &flags_layout);

                state->cc.s = (flags_layout & 0x80)!=0;
                state->cc.z = (flags_layout<<1 & 0x80)!=0;
                state->cc.ac = (flags_layout<<3 & 0x80)!=0;
                state->cc.p = (flags_layout<<5 & 0x80)!=0;
                state->cc.cy = (flags_layout<<7 & 0x80)!=0;
            } break;
        case 0xF2: //^ JP
            {
                if (state->cc.s == 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xF3: //^ DI
            {
                state->interrupt_bus.interrupt_enabled=0;
            } break;
        case 0xF4: unimplementedInstruction(*instr_addr); break;
        case 0xF5: //^ PUSH PSW
            {
                uint8_t flags_layout = ( (state->cc.s << 7) | (state->cc.z << 6) | (state->cc.ac << 4) | (state->cc.p << 2) | 0x02 | (state->cc.cy));
                
                push_16bit_to_stack(state->memory, &(state->sp), combine_two_bytes(state->a, flags_layout));
            } break;
        case 0xF6://^ ORI D8
            {
                uint8_t op = get_operand(instr_addr, 1);
                uint16_t result = state->a | op;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = 0;

                state->a = result;

                state->pc++;
            } break;
        case 0xF7: unimplementedInstruction(*instr_addr); break;
        case 0xF8: //^ RM
            {
                if (state->cc.s) {
                    uint8_t *pc_l = ((uint8_t *) &(state->pc));
                    uint8_t *pc_h = pc_l+1;

                    pop_2bytes_from_stack(state->memory, &(state->sp), pc_h, pc_l);
                }
            } break;
        case 0xF9: //^ SPHL
            {
                state->sp = combine_two_bytes(state->h, state->l);
            } break;
        case 0xFA: //^ JM
            {
                if (state->cc.s != 0) {
                    uint8_t op1 = get_operand(instr_addr, 1);
                    uint8_t op2 = get_operand(instr_addr, 2);
                    uint16_t op_combined = combine_two_bytes(op2, op1);
                    state->pc = op_combined;
                    state->pc--;
                }
                else {
                    state->pc+=2;
                }
            } break;
        case 0xFB: //^ EI
            {
                state->interrupt_bus.interrupt_enabled=1;
            } break;
        case 0xFC: unimplementedInstruction(*instr_addr); break;
        case 0xFD: unimplementedInstruction(*instr_addr); break;
        case 0xFE: //^ CPI
            {
                uint8_t op = get_operand(instr_addr, 1);
                uint16_t result = state->a - op;
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (state->a < op);

                state->pc++;
            } break;
        case 0xFF: //^ RST 7
            {
                uint16_t return_addr = state->pc;
                push_16bit_to_stack(state->memory, &(state->sp), return_addr);

                state->pc = 0x38;
                state->pc--;
            } break;
    }
    
    state->pc++;
}
void cpu_emulation_handler(State8080* state) {
    emulate_i8080_instruction(state);
}