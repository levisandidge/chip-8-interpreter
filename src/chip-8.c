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
#include <time.h>
#include <unistd.h>

#include "../include/decode.h"
#include "../include/file_dialog.h"

typedef struct {
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Texture *texture;
} SDL_GRAPHICS;

void initialize_window(SDL_GRAPHICS *window) {
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

int main(int argc, char *argv[]) {
  // Add a log file for errors
  const char *error_file = "./log.txt";
  FILE *error_fd = freopen(error_file, "w", stderr);
  if (error_fd == NULL) {
    printf("Error Number % d\n", errno);
    perror("Program");
  }

  char *dialog_path = NULL;
  char *file_path;

  // map ROM into memory
  if (argc < 2) {
    // TODO: open a file browser to allow for non terminal usage
    //fprintf(stderr, "Enter a file path to a ROM\n");
    //return 1;
    dialog_path = open_file_dialog();
    if (dialog_path == NULL) {
      fprintf(stderr, "User clicked cancel");
      return 1;
    }
    file_path = dialog_path;
  } else {
    file_path = argv[1];
  }

  int rom_fd = open(file_path, O_RDONLY);
  free(dialog_path);
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
  BYTE *rom = mmap(NULL, rom_info.st_size, PROT_READ, MAP_PRIVATE, rom_fd, 0);
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
  initialize_cpu(&cpu);

  SCREEN screen = {0};

  STACK stack = {0};

  SDL_GRAPHICS window;

  initialize_window(&window);

  bool done = false;

  struct timespec initial;

  clock_gettime(CLOCK_MONOTONIC_RAW, &initial);

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }

    // Fetch the program instruction
    cpu.instruction = RAM[cpu.PC] << 8 | RAM[cpu.PC + 1];

    // Decode and execute the instruction
    decode(&cpu, &screen, &stack, RAM);

    if (cpu.rerender == true) {
      render(&window, &screen);
      cpu.rerender = false;
    }
    
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);

    // decrement timer and buzzer by 1 every second
    if ((now.tv_sec + now.tv_nsec * 1e-9) - (initial.tv_sec + initial.tv_nsec * 1e-9) >= (1.0 / 60)) {
      clock_gettime(CLOCK_MONOTONIC_RAW, &initial);
      if (cpu.DELAY_TIMER > 0)
        cpu.DELAY_TIMER -= 1;
      if (cpu.SOUND_TIMER > 0)
        cpu.SOUND_TIMER -= 1;
    }
  }

  delete_window(&window);

  return 0;
}
