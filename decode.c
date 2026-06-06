#include "include/decode.h"

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
  WORD sum = 0;
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
    sum = cpu->V[X] + cpu->V[Y];
    if (sum > 255) {
      cpu->V[0xF] = 1;
    } else {
      cpu->V[0xF] = 0;
    }
    cpu->V[X] = sum & 0xFF;
    break;
  case 0x5:
    cpu->V[0xF] = 1;
    if (cpu->V[X] < cpu->V[Y])
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
    cpu->V[0xF] = (cpu->V[X] & 0x80) >> 7;
    cpu->V[X] = cpu->V[X] << 1;
    break;
  default:
    fprintf(stderr, "unknown instruction: 0x%X/n", cpu->instruction);
    break;
  }
}

void switch_0xf(BYTE X, WORD NN, CPU *cpu, const bool *keys, BYTE *RAM) {
  switch (NN) {
  case 0x07:
    cpu->V[X] = cpu->DELAY_TIMER;
    break;
  case 0x0A:
    if (!keys[chip8_keys[cpu->V[X]]]) {
      cpu->PC -= 2;
    }
    break;
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
    cpu->I = RAM[cpu->V[X]] * 5; // point I to hex character of V[X]
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

void decode(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE N1 = (cpu->instruction & 0xF000) >> 12;
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  BYTE N = cpu->instruction & 0x000F;

  BYTE NN = cpu->instruction & 0x00FF;
  WORD NNN = cpu->instruction & 0x0FFF;

  cpu->PC += 2;

  const bool *keys = SDL_GetKeyboardState(NULL);

  // TODO: decode and execute within the switch statement
  switch (N1) {
  case 0x0:
    if (NNN == 0x0E0) {
      memset(screen->arr, 0, sizeof(screen->arr));
    } else if (NNN == 0x0EE) {
      ret(stack, cpu);
    }
    break;
  case 0x1:
    cpu->PC = NNN;
    break;
  case 0x2:
    call(stack, cpu, NNN);
    break;
  case 0x3:
    if (cpu->V[X] == NN)
      cpu->PC += 2;
    break;
  case 0x4:
    if (cpu->V[X] != NN)
      cpu->PC += 2;
    break;
  case 0x5:
    if (cpu->V[X] == cpu->V[Y])
      cpu->PC += 2;
    break;
  case 0x6:
    cpu->V[X] = NN;
    break;
  case 0x7:
    cpu->V[X] += NN;
    break;
  case 0x8:
    math(X, Y, N, cpu);
    break;
  case 0x9:
    if (cpu->V[X] != cpu->V[Y])
      cpu->PC += 2;
    break;
  case 0xA:
    cpu->I = NNN;
    break;
  case 0xB:
    cpu->PC = NNN + cpu->V[0];
    break;
  case 0xC:
    cpu->V[X] = (rand() % 256) & NN;
    break;
  case 0xD:
    cpu->rerender = true;
    DXYN(cpu, screen, RAM, X, Y, N);
    break;
  case 0xE:
    switch (NN) {
    case 0x9E:
      if (keys[chip8_keys[cpu->V[X]]]) {
        cpu->PC += 2;
      }
      break;
    case 0xA1:
      if (!keys[chip8_keys[cpu->V[X]]]) {
        cpu->PC += 2;
      }
      break;
    default:
      fprintf(stderr, "(line 537)unknown instruction: 0X%X\n", cpu->instruction);
      break;
    }
    break;
  case 0xF:
    switch_0xf(X, NN, cpu, keys, RAM);
    break;
  }
}
