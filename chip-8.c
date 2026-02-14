#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include <stdlib.h>

#define MAX_SIZE 12

typedef uint8_t BYTE;
typedef int8_t SIGNED_BYTE;
typedef uint16_t WORD;
typedef int16_t SIGNED_WORD;

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

    WORD PC, SP, I;

    BYTE DELAY_TIMER;
    BYTE SOUND_TIMER;

    // registers
    //BYTE V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF;
    BYTE V[16];
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
    //cpu.PC = 0x200;
    cpu.PC = 0x0000;

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
    switch((first & 0xF0) >> 2) {
        case (0x1):
            cpu.PC = ((WORD)(first & 0x0F) << 4) + second;
            break;
        case (0x6):
            cpu.V[first & 0x0F] = second;
            break;
        case (0x7):
            cpu.V[first & 0x0F] += second;
            break;
        case (0xA):
            cpu.I = ((WORD)(first & 0x0F) << 4) + second;
            break;
        case (0xD):
            WORD x = cpu.V[first & 0xF] % 64;
            WORD y = cpu.V[(second & 0xF0) >> 2] % 32;
            BYTE n = second & 0xF;
            BYTE sprite;

            cpu.V[0xF] = 0;

            for (int i = 0; i < n; i++) {
                if (y + i> 31) {
                    continue;
                }

                sprite = rom[cpu.I + i];
                for (int j = 0; j < 8; j++) {
                    if (x + j > 63) {
                        continue;
                    }
                    screen.arr[x+i][y+j] = screen.arr[x+i][y+j] ^ ((sprite >> j) & 0x0001);
                    if (screen.arr[x+i][y+j] == true && (sprite >> j) == 1) {
                        cpu.V[0xF] = 1;
                    }
                }
            }
            break;
    }

    switch (cpu.instruction) {
        case (0x00E0): 
            memset(screen.arr, 0, sizeof(*screen.arr));
            break;
    };

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
            printf("%d ", screen.arr[j][i]);
        }
        printf("\n");
    }
    return 0;
}
