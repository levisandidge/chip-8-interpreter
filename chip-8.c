#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include <stdlib.h>

#define MAX_SIZE 12

typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD;
typedef signed short SIGNED_WORD;

/*
Font:
0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F
*/

typedef struct 
{
    BYTE arr[MAX_SIZE];
} STACK;

typedef struct 
{
    bool arr[64][32];
} SCREEN;

typedef struct
{
    // 2 byte instructions
    WORD  instruction;

    WORD PC;
    WORD SP;
    WORD I;

    BYTE DELAY_TIMER;
    BYTE SOUND_TIMER;

    // registers
    BYTE V0;
    BYTE V1;
    BYTE V2;
    BYTE V3;
    BYTE V4;
    BYTE V5;
    BYTE V6;
    BYTE V7;
    BYTE V8;
    BYTE V9;
    BYTE VA;
    BYTE VB;
    BYTE VC;
    BYTE VD;
    BYTE VE;
    BYTE VF;
} CPU;

// TODO: Setup SDL3

int main(int argc, char* argv[]) {
    // Add a log file for errors
    const char *error_file = "./log.txt";
    FILE *error_fd = freopen(error_file, "a", stderr);
    if (error_fd == NULL) {

        // Print which type of error have in a code
        printf("Error Number % d\n", errno);

        // print program detail "Success or failure"
        perror("Program");
    }

    //TODO: map rom into memory
    if (argc < 2) {
        perror("Enter a file path to a ROM");
        return 1;
    }

    char* file_path = argv[1];
    int rom_fd = open(file_path, O_RDWR);
    if (rom_fd == -1) {
        perror("Failed to open ROM file");
        return 1;
    }

    struct stat rom_info;
    if (fstat(rom_fd, &rom_info) != 0) {
        perror("Could not get ROM info");
        return 1;
    }

    WORD *rom = mmap(NULL, rom_info.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, rom_fd, 0);
    if (rom == MAP_FAILED) {
        perror("MMAP Failed");
        return 1;
    }


    //TODO: setup cpu
    CPU cpu;
    cpu.PC = 0x200;

    SCREEN screen;

    STACK stack;
    cpu.SP = 0;

    // TODO: fetch:
    // Get each nibble out of the instruction to be able to decode instruction

    cpu.instruction = rom[cpu.PC];
    BYTE first = (cpu.instruction & 0xFF00) >> 4;
    BYTE second = cpu.instruction & 0x00FF;

    cpu.PC += 2;

    // TODO: decode and execute within the switch statement
    switch (cpu.instruction) {
        case (0x00E0): {
            memset(screen.arr, 0, sizeof(*screen.arr));
            break;
        }
    };

    return 0;
}