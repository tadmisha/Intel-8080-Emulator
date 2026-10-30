#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct ConditionCodes { //? Intel 8080 flags
    uint8_t z:1;
    uint8_t s:1;
    uint8_t p:1;
    uint8_t cy:1;
    uint8_t ac:1;
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
} State8080;

uint8_t calculate_parity(uint8_t num) {
    num ^= num>>4;
    num ^= num>>2;
    num ^= num>>1;
    return (num & 0b00000001) == 0;
}


void setZSP_Flags(ConditionCodes *cc, const uint16_t result) {
    cc->z = ((result&0xFF) == 0);
    cc->s = ((result & 0b0000000010000000) != 0);
    cc->p = calculate_parity(result&0xFF);
}


void unimplementedInstruction(const unsigned char instr) {
    printf("Error: Unimplemented instruction: %02x\n", instr);
    exit(1);
}

int emulate_i8080_instruction(State8080* state) {
    unsigned char *instr_addr = &(state->memory[state->pc]);

    switch (*instr_addr) {
        case 0x00: break; //^ NOP
        case 0x01: //^ LXI B
        {
            state->b=instr_addr[2];
            state->c=instr_addr[1]; 
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
        case 0x0F: unimplementedInstruction(*instr_addr); break;
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
        case 0x1F: unimplementedInstruction(*instr_addr); break;
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
        case 0x2F: unimplementedInstruction(*instr_addr); break;
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
            state->b = state->c;
            break;
        case 0x42: //^ MOV B,D
            state->b = state->d;
            break;
        case 0x43: //^ MOV B,E
            state->b = state->e;
            break;
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
        case 0x76: unimplementedInstruction(*instr_addr); break;
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
                uint16_t offset = ((uint16_t) state->h << 8) + ((uint16_t) state->l);
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
        case 0xC1: unimplementedInstruction(*instr_addr); break;
        case 0xC2: unimplementedInstruction(*instr_addr); break;
        case 0xC3: unimplementedInstruction(*instr_addr); break;
        case 0xC4: unimplementedInstruction(*instr_addr); break;
        case 0xC5: unimplementedInstruction(*instr_addr); break;
        case 0xC6: //^ ADI
            {
                uint16_t result = (uint16_t) state->a + instr_addr[1];
                setZSP_Flags(&(state->cc), result);
                state->cc.cy = (result > 0xFF);

                state->a=result;

                state->pc++;
            } break;
        case 0xC7: unimplementedInstruction(*instr_addr); break;
        case 0xC8: unimplementedInstruction(*instr_addr); break;
        case 0xC9: unimplementedInstruction(*instr_addr); break;
        case 0xCA: unimplementedInstruction(*instr_addr); break;
        case 0xCB: unimplementedInstruction(*instr_addr); break;
        case 0xCC: unimplementedInstruction(*instr_addr); break;
        case 0xCD: unimplementedInstruction(*instr_addr); break;
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
        case 0xE3: unimplementedInstruction(*instr_addr); break;
        case 0xE4: unimplementedInstruction(*instr_addr); break;
        case 0xE5: unimplementedInstruction(*instr_addr); break;
        case 0xE6: unimplementedInstruction(*instr_addr); break;
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
        case 0xF1: unimplementedInstruction(*instr_addr); break;
        case 0xF2: unimplementedInstruction(*instr_addr); break;
        case 0xF3: unimplementedInstruction(*instr_addr); break;
        case 0xF4: unimplementedInstruction(*instr_addr); break;
        case 0xF5: unimplementedInstruction(*instr_addr); break;
        case 0xF6: unimplementedInstruction(*instr_addr); break;
        case 0xF7: unimplementedInstruction(*instr_addr); break;
        case 0xF8: unimplementedInstruction(*instr_addr); break;
        case 0xF9: unimplementedInstruction(*instr_addr); break;
        case 0xFA: unimplementedInstruction(*instr_addr); break;
        case 0xFB: unimplementedInstruction(*instr_addr); break;
        case 0xFC: unimplementedInstruction(*instr_addr); break;
        case 0xFD: unimplementedInstruction(*instr_addr); break;
        case 0xFE: unimplementedInstruction(*instr_addr); break;
        case 0xFF: unimplementedInstruction(*instr_addr); break;
    }

    state->pc++;
}