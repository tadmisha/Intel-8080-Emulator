#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct ConditionCodes { //? Intel 8080 flags
    uint8_t z:1; //? zero
    uint8_t s:1; //? sign
    uint8_t p:1; //? parity
    uint8_t cy:1; //? carry
    uint8_t ac:1; //? auxiliary carry
    uint8_t pad:3;
} ConditionCodes;

typedef struct State8080 { //? Registers + address of the memory + flags + interrupt enabled/disabled
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    uint8_t int_enable;
    uint8_t cpu_haulted;
} State8080;

uint8_t calculate_parity(uint8_t num) {
    num ^= num>>4;
    num ^= num>>2;
    num ^= num>>1;
    return (num & 0x1) == 0;
}

static inline uint8_t get_operand(const uint8_t *instr_addr, const int index) { //? A get_operand function for better code readability
    return instr_addr[index];
}

static inline uint16_t combine_two_bytes(const uint8_t high_byte, const uint8_t low_byte) { //? Combines 2 8bits into an 16bit
    return ((uint16_t) high_byte<<8) + ((uint16_t) low_byte);
}

static inline void push_16bit_to_stack(uint8_t *memory, uint16_t *sp, uint16_t bits) { //? Pushed 16 bits on the top of the stack
    memory[*sp-1] = (bits >> 8) & 0xff; //! High byte below the low byte on the stack
    memory[*sp-2] = bits & 0xff; //! Low byte on top of the stack
    *sp-=2;
} //! Low byte on sp-2, high byte on sp-1

static inline void pop_2bytes_from_stack(const uint8_t *memory, uint16_t *sp, uint8_t *dst_high_byte, uint8_t *dst_low_byte) { //? Pops 2 bytes from stack and puts into the provided destinations
    *dst_low_byte = memory[*sp];
    *dst_high_byte = memory[*sp+1];
    *sp+=2;
}

static inline void setZSP_Flags(ConditionCodes *cc, const uint16_t result) {
    cc->z = ((result&0xFF) == 0);
    cc->s = ((result & 0x80) != 0);
    cc->p = calculate_parity(result&0xFF);
}


void unimplementedInstruction(const unsigned char instr) {
    printf("Error: Unimplemented instruction: %02x\n", instr);
    exit(1);
}

int emulate_i8080_instruction(State8080* state) {
    uint8_t *instr_addr = state->memory + state->pc;

    switch (*instr_addr) {
        case 0x00: break; //^ NOP
        case 0x01: //^ LXI B
        {
            uint8_t op2=get_operand(instr_addr, 2);
            uint8_t op1=get_operand(instr_addr, 1);
            state->b=op2;
            state->c=op1; 
            state->pc+=2;
        } break;
        case 0x02: unimplementedInstruction(*instr_addr); break;
        case 0x03: unimplementedInstruction(*instr_addr); break;
        case 0x04: unimplementedInstruction(*instr_addr); break;
        case 0x05: unimplementedInstruction(*instr_addr); break;
        case 0x06: unimplementedInstruction(*instr_addr); break;
        case 0x07: unimplementedInstruction(*instr_addr); break;
        case 0x08: unimplementedInstruction(*instr_addr); break;
        case 0x09: unimplementedInstruction(*instr_addr); break;
        case 0x0A: unimplementedInstruction(*instr_addr); break;
        case 0x0B: unimplementedInstruction(*instr_addr); break;
        case 0x0C: unimplementedInstruction(*instr_addr); break;
        case 0x0D: unimplementedInstruction(*instr_addr); break;
        case 0x0E: unimplementedInstruction(*instr_addr); break;
        case 0x0F: //^ RRC
            {
                uint8_t result = (state->a >> 1) | (state->a << 7);
                state->cc.cy = state->a&1;

                state->a = result;
            } break;
        case 0x10: unimplementedInstruction(*instr_addr); break;
        case 0x11: unimplementedInstruction(*instr_addr); break;
        case 0x12: unimplementedInstruction(*instr_addr); break;
        case 0x13: unimplementedInstruction(*instr_addr); break;
        case 0x14: unimplementedInstruction(*instr_addr); break;
        case 0x15: unimplementedInstruction(*instr_addr); break;
        case 0x16: unimplementedInstruction(*instr_addr); break;
        case 0x17: unimplementedInstruction(*instr_addr); break;
        case 0x18: unimplementedInstruction(*instr_addr); break;
        case 0x19: unimplementedInstruction(*instr_addr); break;
        case 0x1A: unimplementedInstruction(*instr_addr); break;
        case 0x1B: unimplementedInstruction(*instr_addr); break;
        case 0x1C: unimplementedInstruction(*instr_addr); break;
        case 0x1D: unimplementedInstruction(*instr_addr); break;
        case 0x1E: unimplementedInstruction(*instr_addr); break;
        case 0x1F: //^ RAR
            {
                uint8_t result = (state->a >> 1) | (state->a & 0x80);
                state->cc.cy = state->a&1;

                state->a = result;
            } break;
        case 0x20: unimplementedInstruction(*instr_addr); break;
        case 0x21: unimplementedInstruction(*instr_addr); break;
        case 0x22: unimplementedInstruction(*instr_addr); break;
        case 0x23: unimplementedInstruction(*instr_addr); break;
        case 0x24: unimplementedInstruction(*instr_addr); break;
        case 0x25: unimplementedInstruction(*instr_addr); break;
        case 0x26: unimplementedInstruction(*instr_addr); break;
        case 0x27: unimplementedInstruction(*instr_addr); break;
        case 0x28: unimplementedInstruction(*instr_addr); break;
        case 0x29: unimplementedInstruction(*instr_addr); break;
        case 0x2A: unimplementedInstruction(*instr_addr); break;
        case 0x2B: unimplementedInstruction(*instr_addr); break;
        case 0x2C: unimplementedInstruction(*instr_addr); break;
        case 0x2D: unimplementedInstruction(*instr_addr); break;
        case 0x2E: unimplementedInstruction(*instr_addr); break;
        case 0x2F: //^ CMA
            {
                state->a = ~state->a;
            } break;
        case 0x30: unimplementedInstruction(*instr_addr); break;
        case 0x31: unimplementedInstruction(*instr_addr); break;
        case 0x32: unimplementedInstruction(*instr_addr); break;
        case 0x33: unimplementedInstruction(*instr_addr); break;
        case 0x34: unimplementedInstruction(*instr_addr); break;
        case 0x35: unimplementedInstruction(*instr_addr); break;
        case 0x36: unimplementedInstruction(*instr_addr); break;
        case 0x37: unimplementedInstruction(*instr_addr); break;
        case 0x38: unimplementedInstruction(*instr_addr); break;
        case 0x39: unimplementedInstruction(*instr_addr); break;
        case 0x3A: unimplementedInstruction(*instr_addr); break;
        case 0x3B: unimplementedInstruction(*instr_addr); break;
        case 0x3C: unimplementedInstruction(*instr_addr); break;
        case 0x3D: unimplementedInstruction(*instr_addr); break;
        case 0x3E: unimplementedInstruction(*instr_addr); break;
        case 0x3F: unimplementedInstruction(*instr_addr); break;
        case 0x40: unimplementedInstruction(*instr_addr); break;
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
        case 0x44: unimplementedInstruction(*instr_addr); break;
        case 0x45: unimplementedInstruction(*instr_addr); break;
        case 0x46: unimplementedInstruction(*instr_addr); break;
        case 0x47: unimplementedInstruction(*instr_addr); break;
        case 0x48: unimplementedInstruction(*instr_addr); break;
        case 0x49: unimplementedInstruction(*instr_addr); break;
        case 0x4A: unimplementedInstruction(*instr_addr); break;
        case 0x4B: unimplementedInstruction(*instr_addr); break;
        case 0x4C: unimplementedInstruction(*instr_addr); break;
        case 0x4D: unimplementedInstruction(*instr_addr); break;
        case 0x4E: unimplementedInstruction(*instr_addr); break;
        case 0x4F: unimplementedInstruction(*instr_addr); break;
        case 0x50: unimplementedInstruction(*instr_addr); break;
        case 0x51: unimplementedInstruction(*instr_addr); break;
        case 0x52: unimplementedInstruction(*instr_addr); break;
        case 0x53: unimplementedInstruction(*instr_addr); break;
        case 0x54: unimplementedInstruction(*instr_addr); break;
        case 0x55: unimplementedInstruction(*instr_addr); break;
        case 0x56: unimplementedInstruction(*instr_addr); break;
        case 0x57: unimplementedInstruction(*instr_addr); break;
        case 0x58: unimplementedInstruction(*instr_addr); break;
        case 0x59: unimplementedInstruction(*instr_addr); break;
        case 0x5A: unimplementedInstruction(*instr_addr); break;
        case 0x5B: unimplementedInstruction(*instr_addr); break;
        case 0x5C: unimplementedInstruction(*instr_addr); break;
        case 0x5D: unimplementedInstruction(*instr_addr); break;
        case 0x5E: unimplementedInstruction(*instr_addr); break;
        case 0x5F: unimplementedInstruction(*instr_addr); break;
        case 0x60: unimplementedInstruction(*instr_addr); break;
        case 0x61: unimplementedInstruction(*instr_addr); break;
        case 0x62: unimplementedInstruction(*instr_addr); break;
        case 0x63: unimplementedInstruction(*instr_addr); break;
        case 0x64: unimplementedInstruction(*instr_addr); break;
        case 0x65: unimplementedInstruction(*instr_addr); break;
        case 0x66: unimplementedInstruction(*instr_addr); break;
        case 0x67: unimplementedInstruction(*instr_addr); break;
        case 0x68: unimplementedInstruction(*instr_addr); break;
        case 0x69: unimplementedInstruction(*instr_addr); break;
        case 0x6A: unimplementedInstruction(*instr_addr); break;
        case 0x6B: unimplementedInstruction(*instr_addr); break;
        case 0x6C: unimplementedInstruction(*instr_addr); break;
        case 0x6D: unimplementedInstruction(*instr_addr); break;
        case 0x6E: unimplementedInstruction(*instr_addr); break;
        case 0x6F: unimplementedInstruction(*instr_addr); break;
        case 0x70: unimplementedInstruction(*instr_addr); break;
        case 0x71: unimplementedInstruction(*instr_addr); break;
        case 0x72: unimplementedInstruction(*instr_addr); break;
        case 0x73: unimplementedInstruction(*instr_addr); break;
        case 0x74: unimplementedInstruction(*instr_addr); break;
        case 0x75: unimplementedInstruction(*instr_addr); break;
        case 0x76: //^ HLT
            {
                state->cpu_haulted=1;
            } break;
        case 0x77: unimplementedInstruction(*instr_addr); break;
        case 0x78: unimplementedInstruction(*instr_addr); break;
        case 0x79: unimplementedInstruction(*instr_addr); break;
        case 0x7A: unimplementedInstruction(*instr_addr); break;
        case 0x7B: unimplementedInstruction(*instr_addr); break;
        case 0x7C: unimplementedInstruction(*instr_addr); break;
        case 0x7D: unimplementedInstruction(*instr_addr); break;
        case 0x7E: unimplementedInstruction(*instr_addr); break;
        case 0x7F: unimplementedInstruction(*instr_addr); break;
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
        case 0x82: unimplementedInstruction(*instr_addr); break;
        case 0x83: unimplementedInstruction(*instr_addr); break;
        case 0x84: unimplementedInstruction(*instr_addr); break;
        case 0x85: unimplementedInstruction(*instr_addr); break;
        case 0x86: //^ ADD M
            {
                uint16_t offset = combine_two_bytes(state->h, state->l);
                uint16_t result = (uint16_t) state->a + state->memory[offset];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;
            } break;
        case 0x87: unimplementedInstruction(*instr_addr); break;
        case 0x88: unimplementedInstruction(*instr_addr); break;
        case 0x89: unimplementedInstruction(*instr_addr); break;
        case 0x8A: unimplementedInstruction(*instr_addr); break;
        case 0x8B: unimplementedInstruction(*instr_addr); break;
        case 0x8C: unimplementedInstruction(*instr_addr); break;
        case 0x8D: unimplementedInstruction(*instr_addr); break;
        case 0x8E: unimplementedInstruction(*instr_addr); break;
        case 0x8F: unimplementedInstruction(*instr_addr); break;
        case 0x90: unimplementedInstruction(*instr_addr); break;
        case 0x91: unimplementedInstruction(*instr_addr); break;
        case 0x92: unimplementedInstruction(*instr_addr); break;
        case 0x93: unimplementedInstruction(*instr_addr); break;
        case 0x94: unimplementedInstruction(*instr_addr); break;
        case 0x95: unimplementedInstruction(*instr_addr); break;
        case 0x96: unimplementedInstruction(*instr_addr); break;
        case 0x97: unimplementedInstruction(*instr_addr); break;
        case 0x98: unimplementedInstruction(*instr_addr); break;
        case 0x99: unimplementedInstruction(*instr_addr); break;
        case 0x9A: unimplementedInstruction(*instr_addr); break;
        case 0x9B: unimplementedInstruction(*instr_addr); break;
        case 0x9C: unimplementedInstruction(*instr_addr); break;
        case 0x9D: unimplementedInstruction(*instr_addr); break;
        case 0x9E: unimplementedInstruction(*instr_addr); break;
        case 0x9F: unimplementedInstruction(*instr_addr); break;
        case 0xA0: unimplementedInstruction(*instr_addr); break;
        case 0xA1: unimplementedInstruction(*instr_addr); break;
        case 0xA2: unimplementedInstruction(*instr_addr); break;
        case 0xA3: unimplementedInstruction(*instr_addr); break;
        case 0xA4: unimplementedInstruction(*instr_addr); break;
        case 0xA5: unimplementedInstruction(*instr_addr); break;
        case 0xA6: unimplementedInstruction(*instr_addr); break;
        case 0xA7: unimplementedInstruction(*instr_addr); break;
        case 0xA8: unimplementedInstruction(*instr_addr); break;
        case 0xA9: unimplementedInstruction(*instr_addr); break;
        case 0xAA: unimplementedInstruction(*instr_addr); break;
        case 0xAB: unimplementedInstruction(*instr_addr); break;
        case 0xAC: unimplementedInstruction(*instr_addr); break;
        case 0xAD: unimplementedInstruction(*instr_addr); break;
        case 0xAE: unimplementedInstruction(*instr_addr); break;
        case 0xAF: unimplementedInstruction(*instr_addr); break;
        case 0xB0: unimplementedInstruction(*instr_addr); break;
        case 0xB1: unimplementedInstruction(*instr_addr); break;
        case 0xB2: unimplementedInstruction(*instr_addr); break;
        case 0xB3: unimplementedInstruction(*instr_addr); break;
        case 0xB4: unimplementedInstruction(*instr_addr); break;
        case 0xB5: unimplementedInstruction(*instr_addr); break;
        case 0xB6: unimplementedInstruction(*instr_addr); break;
        case 0xB7: unimplementedInstruction(*instr_addr); break;
        case 0xB8: unimplementedInstruction(*instr_addr); break;
        case 0xB9: unimplementedInstruction(*instr_addr); break;
        case 0xBA: unimplementedInstruction(*instr_addr); break;
        case 0xBB: unimplementedInstruction(*instr_addr); break;
        case 0xBC: unimplementedInstruction(*instr_addr); break;
        case 0xBD: unimplementedInstruction(*instr_addr); break;
        case 0xBE: unimplementedInstruction(*instr_addr); break;
        case 0xBF: unimplementedInstruction(*instr_addr); break;
        case 0xC0: unimplementedInstruction(*instr_addr); break;
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
        case 0xC3: unimplementedInstruction(*instr_addr); break;
        case 0xC4: unimplementedInstruction(*instr_addr); break;
        case 0xC5: //^ PUSH B
            {
                push_16bit_to_stack(state->memory, state->sp, combine_two_bytes(state->b, state->c));
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
        case 0xC8: unimplementedInstruction(*instr_addr); break;
        case 0xC9: unimplementedInstruction(*instr_addr); break;
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
        case 0xCE: unimplementedInstruction(*instr_addr); break;
        case 0xCF: unimplementedInstruction(*instr_addr); break;
        case 0xD0: unimplementedInstruction(*instr_addr); break;
        case 0xD1: unimplementedInstruction(*instr_addr); break;
        case 0xD2: unimplementedInstruction(*instr_addr); break;
        case 0xD3: unimplementedInstruction(*instr_addr); break;
        case 0xD4: unimplementedInstruction(*instr_addr); break;
        case 0xD5: unimplementedInstruction(*instr_addr); break;
        case 0xD6: unimplementedInstruction(*instr_addr); break;
        case 0xD7: unimplementedInstruction(*instr_addr); break;
        case 0xD8: unimplementedInstruction(*instr_addr); break;
        case 0xD9: unimplementedInstruction(*instr_addr); break;
        case 0xDA: unimplementedInstruction(*instr_addr); break;
        case 0xDB: unimplementedInstruction(*instr_addr); break;
        case 0xDC: unimplementedInstruction(*instr_addr); break;
        case 0xDD: unimplementedInstruction(*instr_addr); break;
        case 0xDE: unimplementedInstruction(*instr_addr); break;
        case 0xDF: unimplementedInstruction(*instr_addr); break;
        case 0xE0: unimplementedInstruction(*instr_addr); break;
        case 0xE1: unimplementedInstruction(*instr_addr); break;
        case 0xE2: unimplementedInstruction(*instr_addr); break;
        case 0xE3: //^ XTHL
            {   
                uint8_t intermediate;

                intermediate = state->memory[state->sp];
                state->memory[state->sp] = state->l;
                state->l = intermediate;

                intermediate = state->memory[state->sp+1];
                state->memory[state->sp] = state->h;
                state->h = intermediate;
            } break;
        case 0xE4: unimplementedInstruction(*instr_addr); break;
        case 0xE5: unimplementedInstruction(*instr_addr); break;
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
        case 0xEA: unimplementedInstruction(*instr_addr); break;
        case 0xEB: unimplementedInstruction(*instr_addr); break;
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
        case 0xF2: unimplementedInstruction(*instr_addr); break;
        case 0xF3: //^ DI
            {
                state->int_enable=0;
            } break;
        case 0xF4: unimplementedInstruction(*instr_addr); break;
        case 0xF5: //^ PUSH PSW
            {
                uint8_t flags_layout = ( (state->cc.s << 7) | (state->cc.z << 6) | (state->cc.ac << 4) | (state->cc.p << 2) | 0x02 | (state->cc.cy));
                state->memory[state->sp-2] = flags_layout;

                
                push_16bit_to_stack(state->memory, &(state->sp), combine_two_bytes(state->a, flags_layout));
            } break;
        case 0xF6: unimplementedInstruction(*instr_addr); break;
        case 0xF7: unimplementedInstruction(*instr_addr); break;
        case 0xF8: unimplementedInstruction(*instr_addr); break;
        case 0xF9: 
            {
                state->sp = combine_two_bytes(state->h, state->b);
            } break;
        case 0xFA: unimplementedInstruction(*instr_addr); break;
        case 0xFB: //^ EI
        {
            state->int_enable=1;
        } break;
        case 0xFC: unimplementedInstruction(*instr_addr); break;
        case 0xFD: unimplementedInstruction(*instr_addr); break;
        case 0xFE: //^ CPI
        {
            uint8_t op = get_operand(instr_addr, 1);
            uint16_t result = state->a - op;
            setZSP_Flags(&(state->cc), result);
            state->cc.cy = (state-> a < op);

            state->pc++;
        } break;
        case 0xFF: unimplementedInstruction(*instr_addr); break;
    }

    state->pc++;
}