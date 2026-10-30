#include "disassembler.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char *read_file_bytes(const char *filename, size_t *fsize_addr) { //? Reads file bytes
    FILE *file= fopen(filename, "rb");
    if (file==NULL) {
        printf("Couldn't open the file\n");
        exit(1);
    }
    fseek(file, 0L, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    unsigned char *buffer=calloc(fsize+2, sizeof(unsigned char)); //! +2 because the maximum size of operands in 2 bytes, it's for a case where the last byte is (by mistake) an operator that requires operands, but they arent provided

    fread(buffer, fsize, 1, file);
    fclose(file);

    *fsize_addr = fsize;
    return buffer;
}


int main(int argc, char *argv[]) {
    const char *filename;

    if (argc==2) {
        filename=argv[1];
    }
    else {
        if (argc==1) {
            printf("Filename was not provided.\n");
            exit(1); 
        }
        else {
            printf("Too many arguments provided.\n");
            exit(1);
        }
    }
    
    size_t filesize;
    unsigned char *buffer = read_file_bytes(filename, &filesize);

    int pc = 0;
    while (pc < filesize) {
        pc += disassemble_i8080_instruction(buffer, pc);
    }

    free(buffer);

    return 0;
}