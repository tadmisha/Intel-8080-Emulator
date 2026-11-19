#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>
#include <stdint.h>

typedef struct ConditionCodes { //? Intel 8080 flags
    uint8_t z:1; //? zero
    uint8_t s:1; //? sign
    uint8_t p:1; //? parity
    uint8_t cy:1; //? carry
    uint8_t ac:1; //? auxiliary carry
    uint8_t pad:3;
} ConditionCodes;

typedef struct IO_Port { //? An IO port storage
    uint8_t inputed_data;
    uint8_t outputed_data;
} IO_Port;

typedef struct IO_Bus { //? The collection of ports with the logic that enables connecting the CPU emulator to the rest of the emulated machine
    struct IO_Port io_ports[256]; //? Ports
    uint8_t did_read; //? boolean flag for when sth is inputted
    uint8_t did_write; //? boolean flag for when sth is outputed
    uint8_t used_port_id; //? id of the port that sth has inputted/outputed in
} IO_Bus;

typedef struct Interrupt_Bus {
    uint8_t interrupt_enabled;
    uint8_t did_interrupt;
    uint8_t interrupt_operand;
} Interrupt_Bus;

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
    struct Interrupt_Bus interrupt_bus;
    struct IO_Bus io_bus;
    uint8_t cpu_haulted;
} State8080;

static inline uint8_t calculate_parity(uint8_t num) { //? Calculate Parity
    num ^= num>>4;
    num ^= num>>2;
    num ^= num>>1;
    return (num & 0x1) == 0;
}

static inline uint8_t get_operand(const uint8_t *instr_addr, const int index) { //? A get_operand function for better code readability
    return instr_addr[index];
}

static inline uint16_t combine_two_bytes(const uint8_t high_byte, const uint8_t low_byte){ //? Combines 2 8bits into an 16bit
    return ((uint16_t) high_byte<<8) + ((uint16_t) low_byte);
}

static inline void push_16bit_to_stack(uint8_t *memory, uint16_t *sp, uint16_t bits) { //? Pushed 16 bits on the top of the stack
    memory[*sp-1] = (bits >> 8) & 0xff; //! High byte below the low byte on the stack
    memory[*sp-2] = bits & 0xff; //! Low byte on top of the stack
    *sp-=2;
} //! Low byte on sp-2, high byte on sp-1
    
static inline void put_16bit_in_2_bytes(uint8_t *high_byte, uint8_t *low_byte, const uint16_t bit_16) { //? Divides 16bit into 2 bytes and puts into a register pair
    *high_byte = bit_16>>8;
    *low_byte = bit_16&0xFF;
}

static inline void pop_2bytes_from_stack(const uint8_t *memory, uint16_t *sp, uint8_t *dst_high_byte, uint8_t *dst_low_byte) { //? Pops 2 bytes from stack and puts into the provided destinations
    *dst_low_byte = memory[*sp];
    *dst_high_byte = memory[*sp+1];
    *sp+=2;
}

static inline void setZSP_Flags(ConditionCodes *cc, const uint16_t result) { //? Set zero, sign, parity flags
    cc->z = ((result&0xFF) == 0);
    cc->s = ((result & 0x80) != 0);
    cc->p = calculate_parity(result&0xFF);
}

void initializeI8080(State8080 **state_ptr); //? Set up the initial state of the CPU

void unimplementedInstruction(const unsigned char instr); //? Gets called when an instruction is not implemented in the emulator

void emulate_i8080_instruction(State8080* state); //? Responsible for emulating each separate instruction

void emulation_handler(State8080* state); //? Basic emulation loop

#endif