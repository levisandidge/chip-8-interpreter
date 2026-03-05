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

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define PROGRAM_START 0x200
#define MAX_SIZE 16
#define RAM_SIZE 4096

typedef uint8_t BYTE;
typedef int8_t SIGNED_BYTE;
typedef uint16_t WORD;
typedef int16_t SIGNED_WORD;

// TODO: Setup SDL3

typedef struct {
    SDL_Window* win;
    SDL_Renderer* ren;
} SDL_GRAPHICS;

void initilize_window(SDL_GRAPHICS* window) {
    SDL_Init(SDL_INIT_VIDEO);

    window->win = SDL_CreateWindow("CHIP-8", 640, 320, SDL_WINDOW_OPENGL | SDL_WINDOW_ALWAYS_ON_TOP);

    window->ren = SDL_CreateRenderer(window->win, NULL);

    SDL_SetRenderDrawColor(window->ren, 0, 0, 0, 255);
    SDL_RenderClear(window->ren);
    SDL_RenderPresent(window->ren); // Show the black screen

    bool done = false;

    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        // Do game logic, present a frame, etc.
    }

}

void render(SDL_GRAPHICS* window) {


    SDL_RenderPresent(window->ren);
}

void delete_window(SDL_GRAPHICS* window) {
    SDL_DestroyRenderer(window->ren);

    SDL_DestroyWindow(window->win);

    SDL_Quit();
}

BYTE font[] = {
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
};

typedef struct {
    WORD arr[MAX_SIZE];
} STACK;

typedef struct {
    bool arr[64][32];
} SCREEN;

typedef struct {
    // 2 byte instructions
    WORD instruction;

    WORD PC, SP, I;

    BYTE DELAY_TIMER;
    BYTE SOUND_TIMER;

    // registers
    //BYTE V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF;
    BYTE V[16];
} CPU;

void call(STACK* stack, CPU* cpu, WORD NNN) {
    stack->arr[cpu->SP] = cpu->PC;
    cpu->SP++;
    cpu->PC = NNN;
}

WORD ret(STACK* stack, CPU* cpu) {
    cpu->SP--;
    return stack->arr[cpu->SP];
}

void DXYN(CPU *cpu, SCREEN *screen, BYTE *RAM, BYTE X, BYTE Y, BYTE N) {
    WORD x = cpu->V[X] % 64;
    WORD y = cpu->V[Y] % 32;
    BYTE sprite;

    cpu->V[0xF] = 0;

    for (int i = 0; i < N; i++) {
        if (y + i > 31) {
            continue;
        }

        sprite = RAM[cpu->I + i];
        for (int j = 0; j < 8; j++) {
            if (x + j > 63) {
                continue;
            }
            screen->arr[x+j][y+i] = screen->arr[x + j][y + i] ^ ((sprite >> j) & 0x0001);
            if (screen->arr[x+i][y+j] == true && (sprite >> j) == 1) {
                cpu->V[0xF] = 1;
            }
        }
    }
}

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

    //TODO: map ROM into memory
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

    // TODO: Fix by calloc 4k of memory and memcopy rom into memory
    // TODO: Map font into ram as well
    //BYTE *RAM = calloc(RAM_SIZE, sizeof(BYTE));
    BYTE RAM[RAM_SIZE] = {0};
    WORD *rom = mmap(NULL, rom_info.st_size, PROT_READ, MAP_PRIVATE, rom_fd, 0);
    if (rom == MAP_FAILED) {
        perror("MMAP Failed");
        close(rom_fd);
        return 1;
    }
    close(rom_fd);
    memcpy(RAM, &font, sizeof(font));
    memcpy(RAM + PROGRAM_START, rom,rom_info.st_size);
    munmap(rom, rom_info.st_size);

    //TODO: setup cpu
    CPU cpu;
    cpu.PC = PROGRAM_START;

    SCREEN screen = {0};

    STACK stack = {0};
    cpu.SP = 0;

    SDL_GRAPHICS window;

   initilize_window(&window);

    for (int i = 0; i < 42; i++) {
         // TODO: fetch:
        // Get each nibble out of the instruction to be able to decode instruction
        cpu.instruction = RAM[cpu.PC] << 8 | RAM[cpu.PC + 1];
        //cpu.instruction = __builtin_bswap16(cpu.instruction);
        //printf("%#04X\n", cpu.instruction);

        BYTE N1 = (cpu.instruction & 0xF000) >> 12;
        BYTE X = (cpu.instruction & 0x0F00) >> 8;
        BYTE Y = (cpu.instruction & 0x00F0) >> 4;
        BYTE N = cpu.instruction & 0x000F;
        
        BYTE NN = cpu.instruction & 0x00FF;
        WORD NNN = cpu.instruction & 0x0FFF;

        printf("%X\n", cpu.instruction);

        cpu.PC += 2;

        // TODO: decode and execute within the switch statement
        switch(N1) {
            case 0x0:
                if (NNN == 0x0E0) {
                    memset(screen.arr, 0, sizeof(screen.arr));
                }
                break;
            case 0x1:
                cpu.PC = NNN;
                break;
            case 0x6:
                cpu.V[X] = NN;
                break;
            case 0x7:
                cpu.V[X] += NN;
                break;
            case 0xA:
                cpu.I = NNN;
                break;
            case 0xD:
                DXYN(&cpu, &screen, RAM, X, Y, N);
                break;
        }
    }
    
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
            printf("%d ", screen.arr[j][i]);
        }
        printf("\n");
    }

    delete_window(&window);

    return 0;
}
