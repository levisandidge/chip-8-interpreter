#include "../include/instructions.h"

void OP_00E0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  memset(screen->arr, 0, sizeof(screen->arr));
}

void OP_00EE(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  ret(stack, cpu);
}

void OP_1NNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  WORD NNN = cpu->instruction & 0x0FFF;
  cpu->PC = NNN;
}

void OP_2NNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  WORD NNN = cpu->instruction & 0x0FFF;
  call(stack, cpu, NNN);
}

void OP_3XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE NN = cpu->instruction & 0x00FF;
  if (cpu->V[X] == NN)
    cpu->PC += 2;
}

void OP_4XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE NN = cpu->instruction & 0x00FF;
  if (cpu->V[X] != NN)
    cpu->PC += 2;
}

void OP_5XY0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  if (cpu->V[X] == cpu->V[Y])
    cpu->PC += 2;
}

void OP_6XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE NN = cpu->instruction & 0x00FF;
  cpu->V[X] = NN;
}

void OP_7XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE NN = cpu->instruction & 0x00FF;
  cpu->V[X] += NN;
}

void OP_8XY0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[X] = cpu->V[Y];
}

void OP_8XY1(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[X] = cpu->V[X] | cpu->V[Y];
}

void OP_8XY2(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[X] = cpu->V[X] & cpu->V[Y];
}

void OP_8XY3(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[X] = cpu->V[X] ^ cpu->V[Y];
}

void OP_8XY4(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  WORD sum = cpu->V[X] + cpu->V[Y];
  if (sum > 255) {
    cpu->V[0xF] = 1;
  } else {
    cpu->V[0xF] = 0;
  }
  cpu->V[X] = sum & 0xFF;
}

void OP_8XY5(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  BYTE diff = cpu->V[X] - cpu->V[Y];
  if (cpu->V[X] > cpu->V[Y]) {
     cpu->V[0xF] = 1;
  } else {
    cpu->V[0xF] = 0;
  }
  cpu->V[X] = diff;
}

void OP_8XY6(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  //BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[0xF] = cpu->V[X] & 0x01;
  cpu->V[X] = cpu->V[X] >> 1;
}

void OP_8XY7(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[0xF] = 1;
  if ((int)cpu->V[Y] - (int)cpu->V[X] < 0)
    cpu->V[0xF] = 0;
  cpu->V[X] = cpu->V[Y] - cpu->V[X];
}

void OP_8XYE(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  //BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  cpu->V[0xF] = (cpu->V[X] & 0x80) >> 7;
  cpu->V[X] = cpu->V[X] << 1;
}

void OP_9XY0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  if (cpu->V[X] != cpu->V[Y])
    cpu->PC += 2;
}

void OP_ANNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  WORD NNN = cpu->instruction & 0x0FFF;
  cpu->I = NNN;
}

void OP_BNNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  WORD NNN = cpu->instruction & 0x0FFF;
  cpu->PC = NNN + cpu->V[0];
}

void OP_CXNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE NN = cpu->instruction & 0x00FF;
  cpu->V[X] = (rand() % 256) & NN;
}

void OP_DXYN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  BYTE Y = (cpu->instruction & 0x00F0) >> 4;
  BYTE N = cpu->instruction & 0x000F;
  WORD x = cpu->V[X] % 64;
  WORD y = cpu->V[Y] % 32;
  BYTE sprite;

  cpu->rerender = true;
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

void OP_EX9E(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  const bool *keys = SDL_GetKeyboardState(NULL);
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  if (keys[chip8_keys[cpu->V[X]]]) {
    cpu->PC += 2;
  }
}

void OP_EXA1(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  const bool *keys = SDL_GetKeyboardState(NULL);
    BYTE X = (cpu->instruction & 0x0F00) >> 8;
  if (!keys[chip8_keys[cpu->V[X]]]) {
    cpu->PC += 2;
  }
}

void OP_FX07(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  cpu->V[X] = cpu->DELAY_TIMER;
}

void OP_FX0A(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  const bool *keys = SDL_GetKeyboardState(NULL);
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  if (!keys[chip8_keys[cpu->V[X]]]) {
    cpu->PC -= 2;
  }
}

void OP_FX15(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  cpu->DELAY_TIMER = cpu->V[X];
}

void OP_FX18(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  cpu->SOUND_TIMER = cpu->V[X];
}

void OP_FX1E(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  cpu->I += cpu->V[X];
}

void OP_FX29(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  cpu->I = RAM[cpu->V[X]] * 5; // point I to hex character of V[X]
}

void OP_FX33(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  RAM[cpu->I] = cpu->V[X] / 100;
  RAM[cpu->I + 1] = cpu->V[X] / 10 % 10;
  RAM[cpu->I + 2] = cpu->V[X] % 10;
}

void OP_FX55(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  for (int i = 0; i <= X; i++) {
    RAM[cpu->I + i] = cpu->V[i];
  }
}

void OP_FX65(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE X = (cpu->instruction & 0x0F00) >> 8;
  for (int i = 0; i <= X; i++) {
    cpu->V[i] = RAM[cpu->I + i];
  }
}