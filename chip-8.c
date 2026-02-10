#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

//#include <stdlib.h>

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
    bool arr[64][32]
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


int main(int argc, char* argv)
{
    // Add a log file for errors
    const char* error_file = "./log.txt";
    FILE *rom_fd = freopen(error_file, "a", STDERR_FILENO);
    if (rom_fd == NULL) {

        // Print which type of error have in a code
        printf("Error Number % d\n", errno);

        // print program detail "Success or failure"
        perror("Program");
    }

    //TODO: map rom into memory
    if (argc < 2) {
        return 1;
    } 
    char* file_path = argv[1];

    //TODO: setup cpu
    CPU cpu;
    cpu.PC = 0x200;

    //fetch

    cpu.instruction = 
    cpu.PC += 2;

    //decode
    switch (cpu.instruction) {

    };

    //execute


    return 0;
}