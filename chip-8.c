#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define PROGRAM_START 0x200
#define STACK_SIZE 16
#define RAM_SIZE 4096
#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH 1280

typedef uint8_t BYTE;
typedef int8_t SIGNED_BYTE;
typedef uint16_t WORD;
typedef int16_t SIGNED_WORD;

typedef struct {
  WORD arr[STACK_SIZE];
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
  // BYTE V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF;
  BYTE V[16];
} CPU;

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
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *texture;
} SDL_GRAPHICS;

void initilize_window(SDL_GRAPHICS *window) {
  // TODO: Set up audio
  SDL_Init(SDL_INIT_VIDEO);

  window->win = SDL_CreateWindow("CHIP-8", WINDOW_WIDTH, WINDOW_HEIGHT,
                                 SDL_WINDOW_RESIZABLE);
  // window->win = SDL_CreateWindow("CHIP-8", 640, 320, SDL_WINDOW_OPENGL |
  // SDL_WINDOW_ALWAYS_ON_TOP);

  window->ren = SDL_CreateRenderer(window->win, NULL);

  window->texture = SDL_CreateTexture(window->ren, SDL_PIXELFORMAT_RGBA4444,
                                      SDL_TEXTUREACCESS_STREAMING, 64, 32);

  SDL_SetTextureScaleMode(window->texture, SDL_SCALEMODE_NEAREST);

  SDL_SetRenderDrawColor(window->ren, 0, 0, 0, 255);
  SDL_RenderClear(window->ren);
  SDL_RenderPresent(window->ren); // Show the black screen
}

void render(SDL_GRAPHICS *window, SCREEN *screen) {
  void *mPixels;
  int pitch;

  SDL_LockTexture(window->texture, NULL, &mPixels, &pitch);

  WORD *pixels = (WORD *)mPixels;

  for (int x = 0; x < 64; x++) {
    for (int y = 0; y < 32; y++) {
      if (screen->arr[x][y] == 1) {
        pixels[y * (pitch / 2) + x] = 0xFFFF;
      } else {
        pixels[y * (pitch / 2) + x] = 0x0000;
      }
    }
  }

  SDL_UnlockTexture(window->texture);
  SDL_RenderClear(window->ren);
  SDL_RenderTexture(window->ren, window->texture, NULL, NULL);
  SDL_RenderPresent(window->ren);
}

void delete_window(SDL_GRAPHICS *window) {
  SDL_DestroyRenderer(window->ren);

  SDL_DestroyWindow(window->win);

  SDL_Quit();
}

void call(STACK *stack, CPU *cpu, WORD NNN) {
  stack->arr[cpu->SP] = cpu->PC;
  cpu->SP++;
  cpu->PC = NNN;
}

void ret(STACK *stack, CPU *cpu) {
  cpu->SP--;
  cpu->PC = stack->arr[cpu->SP];
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
      screen->arr[x + j][y + i] =
          screen->arr[x + j][y + i] ^ ((sprite >> (7 - j)) & 0x0001);
      if (screen->arr[x + j][y + i] == true && (sprite >> (7 - j)) == 1) {
        cpu->V[0xF] = 1;
      }
    }
  }
}

void math(BYTE X, BYTE Y, BYTE N, CPU *cpu) {
  switch (N) {
  case 0x0:
    cpu->V[X] = cpu->V[Y];
    break;
  case 0x1:
    cpu->V[X] = cpu->V[X] | cpu->V[Y];
    break;
  case 0x2:
    cpu->V[X] = cpu->V[X] & cpu->V[Y];
    break;
  case 0x3:
    cpu->V[X] = cpu->V[X] ^ cpu->V[Y];
    break;
  case 0x4:
    if ((((int)cpu->V[X] + (int)cpu->V[Y]) >> 7) == 1)
      cpu->V[0xF] = 1;
    cpu->V[X] += cpu->V[Y];
    break;
  case 0x5:
    cpu->V[0xF] = 1;
    if ((int)cpu->V[X] - (int)cpu->V[Y] < 0)
      cpu->V[0xF] = 0;
    cpu->V[X] -= cpu->V[Y];
    break;
  case 0x6:
    cpu->V[0xF] = cpu->V[X] & 0x01;
    cpu->V[X] = cpu->V[X] >> 1;
    break;
  case 0x7:
    cpu->V[0xF] = 1;
    if ((int)cpu->V[Y] - (int)cpu->V[X] < 0)
      cpu->V[0xF] = 0;
    cpu->V[X] = cpu->V[Y] - cpu->V[X];
    break;
  case 0xE:
    cpu->V[0xF] = cpu->V[X] & 0x80 >> 3;
    cpu->V[X] = cpu->V[X] << 1;
    break;
  default:
    fprintf(stderr, "unknown instruction: 0x%X/n", cpu->instruction);
    break;
  }
}

void key_not_pressed(BYTE X, CPU *cpu, const bool *keys) {
  switch (cpu->V[X]) {
  case 0x0:
    if (keys[SDL_SCANCODE_X])
      cpu->PC += 2;
    break;
  case 0x1:
    if (keys[SDL_SCANCODE_1])
      cpu->PC += 2;
    break;
  case 0x2:
    if (keys[SDL_SCANCODE_2])
      cpu->PC += 2;
    break;
  case 0x3:
    if (keys[SDL_SCANCODE_3])
      cpu->PC += 2;
    break;
  case 0x4:
    if (keys[SDL_SCANCODE_Q])
      cpu->PC += 2;
    break;
  case 0x5:
    if (keys[SDL_SCANCODE_W])
      cpu->PC += 2;
    break;
  case 0x6:
    if (keys[SDL_SCANCODE_E])
      cpu->PC += 2;
    break;
  case 0x7:
    if (keys[SDL_SCANCODE_A])
      cpu->PC += 2;
    break;
  case 0x8:
    if (keys[SDL_SCANCODE_S])
      cpu->PC += 2;
    break;
  case 0x9:
    if (keys[SDL_SCANCODE_D])
      cpu->PC += 2;
    break;
  case 0xA:
    if (keys[SDL_SCANCODE_Z])
      cpu->PC += 2;
    break;
  case 0xB:
    if (keys[SDL_SCANCODE_C])
      cpu->PC += 2;
    break;
  case 0xC:
    if (keys[SDL_SCANCODE_4])
      cpu->PC += 2;
    break;
  case 0xD:
    if (keys[SDL_SCANCODE_R])
      cpu->PC += 2;
    break;
  case 0xE:
    if (keys[SDL_SCANCODE_F])
      cpu->PC += 2;
    break;
  case 0xF:
    if (keys[SDL_SCANCODE_V])
      cpu->PC += 2;
    break;
  default:
    fprintf(stderr, "unknown instruction: 0X%X\n", cpu->instruction);
    break;
  }
}

void key_pressed(BYTE X, CPU *cpu, const bool *keys) {
  switch (cpu->V[X]) {
  case 0x0:
    if (!keys[SDL_SCANCODE_X])
      cpu->PC += 2;
    break;
  case 0x1:
    if (!keys[SDL_SCANCODE_1])
      cpu->PC += 2;
    break;
  case 0x2:
    if (!keys[SDL_SCANCODE_2])
      cpu->PC += 2;
    break;
  case 0x3:
    if (!keys[SDL_SCANCODE_3])
      cpu->PC += 2;
    break;
  case 0x4:
    if (!keys[SDL_SCANCODE_Q])
      cpu->PC += 2;
    break;
  case 0x5:
    if (!keys[SDL_SCANCODE_W])
      cpu->PC += 2;
    break;
  case 0x6:
    if (!keys[SDL_SCANCODE_E])
      cpu->PC += 2;
    break;
  case 0x7:
    if (!keys[SDL_SCANCODE_A])
      cpu->PC += 2;
    break;
  case 0x8:
    if (!keys[SDL_SCANCODE_S])
      cpu->PC += 2;
    break;
  case 0x9:
    if (!keys[SDL_SCANCODE_D])
      cpu->PC += 2;
    break;
  case 0xA:
    if (!keys[SDL_SCANCODE_Z])
      cpu->PC += 2;
    break;
  case 0xB:
    if (!keys[SDL_SCANCODE_C])
      cpu->PC += 2;
    break;
  case 0xC:
    if (!keys[SDL_SCANCODE_4])
      cpu->PC += 2;
    break;
  case 0xD:
    if (!keys[SDL_SCANCODE_R])
      cpu->PC += 2;
    break;
  case 0xE:
    if (!keys[SDL_SCANCODE_F])
      cpu->PC += 2;
    break;
  case 0xF:
    if (!keys[SDL_SCANCODE_V])
      cpu->PC += 2;
    break;
  default:
    fprintf(stderr, "(line 520)unknown instruction: 0X%X\n", cpu->instruction);
    break;
  }
}

void switch_0xf(BYTE X, WORD NN, CPU *cpu, const bool *keys, BYTE *RAM) {
  switch (NN) {
  case 0x07:
    cpu->V[X] = cpu->DELAY_TIMER;
    break;
  case 0x0A:
    switch (cpu->V[X]) {
    case 0x0:
      if (!keys[SDL_SCANCODE_X])
        cpu->PC -= 2;
      break;
    case 0x1:
      if (!keys[SDL_SCANCODE_1])
        cpu->PC -= 2;
      break;
    case 0x2:
      if (!keys[SDL_SCANCODE_2])
        cpu->PC -= 2;
      break;
    case 0x3:
      if (!keys[SDL_SCANCODE_3])
        cpu->PC -= 2;
      break;
    case 0x4:
      if (!keys[SDL_SCANCODE_Q])
        cpu->PC -= 2;
      break;
    case 0x5:
      if (!keys[SDL_SCANCODE_W])
        cpu->PC -= 2;
      break;
    case 0x6:
      if (!keys[SDL_SCANCODE_E])
        cpu->PC -= 2;
      break;
    case 0x7:
      if (!keys[SDL_SCANCODE_A])
        cpu->PC -= 2;
      break;
    case 0x8:
      if (!keys[SDL_SCANCODE_S])
        cpu->PC -= 2;
      break;
    case 0x9:
      if (!keys[SDL_SCANCODE_D])
        cpu->PC -= 2;
      break;
    case 0xA:
      if (!keys[SDL_SCANCODE_Z])
        cpu->PC -= 2;
      break;
    case 0xB:
      if (!keys[SDL_SCANCODE_C])
        cpu->PC -= 2;
      break;
    case 0xC:
      if (!keys[SDL_SCANCODE_4])
        cpu->PC -= 2;
      break;
    case 0xD:
      if (!keys[SDL_SCANCODE_R])
        cpu->PC -= 2;
      break;
    case 0xE:
      if (!keys[SDL_SCANCODE_F])
        cpu->PC -= 2;
      break;
    case 0xF:
      if (!keys[SDL_SCANCODE_V]) {
        cpu->PC -= 2;
      }
      break;
    default:
      fprintf(stderr, "unknown instruction: 0X%X\n", cpu->instruction);
      break;
    }
  case 0x15:
    cpu->DELAY_TIMER = cpu->V[X];
    break;
  case 0x18:
    cpu->SOUND_TIMER = cpu->V[X];
    break;
  case 0x1E:
    cpu->I += cpu->V[X];
    break;
  case 0x29:
    cpu->I = RAM[cpu->V[X] * 5]; // point I to hex character of V[X]
    break;
  case 0x33:
    RAM[cpu->I] = cpu->V[X] / 100;
    RAM[cpu->I + 1] = cpu->V[X] / 10 % 10;
    RAM[cpu->I + 2] = cpu->V[X] % 10;
    break;
  case 0x55:
    for (int i = 0; i <= X; i++) {
      RAM[cpu->I + i] = cpu->V[i];
    }
    break;
  case 0x65:
    for (int i = 0; i <= X; i++) {
      cpu->V[i] = RAM[cpu->I + i];
    }
    break;
  default:
    fprintf(stderr, "unknown instruction: 0X%X\n", cpu->instruction);
    break;
  }
}

int main(int argc, char *argv[]) {
  // Add a log file for errors
  const char *error_file = "./log.txt";
  FILE *error_fd = freopen(error_file, "w", stderr);
  if (error_fd == NULL) {
    printf("Error Number % d\n", errno);
    perror("Program");
  }

  char *file_path;

  // map ROM into memory
  if (argc < 2) {
    // TODO: open a file browser to allow for non terminal usage
    perror("Enter a file path to a ROM");
    return 1;
  } else {
    file_path = argv[1];
  }

  int rom_fd = open(file_path, O_RDONLY);
  if (rom_fd == -1) {
    perror("Failed to open ROM file");
    return 1;
  }

  struct stat rom_info;
  if (fstat(rom_fd, &rom_info) != 0) {
    perror("Could not get ROM info");
    return 1;
  }

  BYTE RAM[RAM_SIZE] = {0};
  WORD *rom = mmap(NULL, rom_info.st_size, PROT_READ, MAP_PRIVATE, rom_fd, 0);
  if (rom == MAP_FAILED) {
    perror("MMAP Failed");
    close(rom_fd);
    return 1;
  }
  close(rom_fd);
  memcpy(RAM, &font, sizeof(font));
  memcpy(RAM + PROGRAM_START, rom, rom_info.st_size);
  munmap(rom, rom_info.st_size);

  // setup cpu
  CPU cpu;
  cpu.PC = PROGRAM_START;

  SCREEN screen = {0};

  STACK stack = {0};
  cpu.SP = 0;

  SDL_GRAPHICS window;

  initilize_window(&window);

  bool done = false;
  bool rerender = false;

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }

    // Get the program instruction
    cpu.instruction = RAM[cpu.PC] << 8 | RAM[cpu.PC + 1];

    BYTE N1 = (cpu.instruction & 0xF000) >> 12;
    BYTE X = (cpu.instruction & 0x0F00) >> 8;
    BYTE Y = (cpu.instruction & 0x00F0) >> 4;
    BYTE N = cpu.instruction & 0x000F;

    BYTE NN = cpu.instruction & 0x00FF;
    WORD NNN = cpu.instruction & 0x0FFF;

    cpu.PC += 2;

    const bool *keys = SDL_GetKeyboardState(NULL);

    // TODO: decode and execute within the switch statement
    switch (N1) {
    case 0x0:
      if (NNN == 0x0E0) {
        memset(screen.arr, 0, sizeof(screen.arr));
      } else if (NNN == 0x0EE) {
        ret(&stack, &cpu);
      }
      break;
    case 0x1:
      cpu.PC = NNN;
      break;
    case 0x2:
      call(&stack, &cpu, NNN);
      break;
    case 0x3:
      if (cpu.V[X] == NN)
        cpu.PC += 2;
      break;
    case 0x4:
      if (cpu.V[X] != NN)
        cpu.PC += 2;
      break;
    case 0x5:
      if (cpu.V[X] == cpu.V[Y])
        cpu.PC += 2;
      break;
    case 0x6:
      cpu.V[X] = NN;
      break;
    case 0x7:
      cpu.V[X] += NN;
      break;
    case 0x8:
      math(X, Y, N, &cpu);
      break;
    case 0x9:
      if (cpu.V[X] != cpu.V[Y])
        cpu.PC += 2;
      break;
    case 0xA:
      cpu.I = NNN;
      break;
    case 0xB:
      cpu.PC = NNN + cpu.V[0];
      break;
    case 0xC:
      cpu.V[X] = (rand() % 255) & NN;
      break;
    case 0xD:
      rerender = true;
      DXYN(&cpu, &screen, RAM, X, Y, N);
      break;
    case 0xE:
      switch (NN) {
      case 0x9E:
        key_not_pressed(X, &cpu, keys);
        break;
      case 0xA1:
        key_pressed(X, &cpu, keys);
        break;
      default:
        fprintf(stderr, "(line 537)unknown instruction: 0X%X\n",
                cpu.instruction);
        break;
      }
      break;
    case 0xF:
      switch_0xf(X, NN, &cpu, keys, RAM);
      break;
    }

    if (rerender == true) {
      render(&window, &screen);
      rerender = false;
    }
  }

  // add timing and decrement timer and buzzer by 1 every second

  delete_window(&window);

  return 0;
}
