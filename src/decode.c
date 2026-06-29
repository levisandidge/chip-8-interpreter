#include "../include/decode.h"

void OP_0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  if (cpu->instruction == 0x00E0) {
    OP_0_Table[0](cpu, screen, stack, RAM);
  } else if (cpu->instruction == 0x00EE) {
    OP_0_Table[1](cpu, screen, stack, RAM);
  } else {
    fprintf(stderr, "Unknown instruction: %x", cpu->instruction);
  }
}

void CPU_ARITHMETIC(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  if ((cpu->instruction & 0x000F) < 8) {
    CPU_ARITHMETIC_Table[cpu->instruction & 0x000F](cpu, screen, stack, RAM);
  } else if ((cpu->instruction & 0x000F) == 0xE) {
    CPU_ARITHMETIC_Table[8](cpu, screen, stack, RAM);
  } else {
    fprintf(stderr, "unknown instruction: %x", cpu->instruction);
  }
}

void KEY_PRESSED(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  if ((cpu->instruction & 0x00FF) == 0x009E) {
    KEY_PRESSED_Table[0](cpu, screen, stack, RAM);
  } else if ((cpu->instruction & 0x00FF) == 0x00A1) {
    KEY_PRESSED_Table[1](cpu, screen, stack, RAM);
  } else {
    fprintf(stderr, "Unknown instruction: %x", cpu->instruction);
  }
}

void CPU_REGISTERS(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  BYTE NN = cpu->instruction & 0x00FF;
  switch (NN) {
  case 0x07:
    CPU_REGISTERS_Table[0](cpu, screen, stack, RAM);
    break;
  case 0x0A:
    CPU_REGISTERS_Table[1](cpu, screen, stack, RAM);
    break;
  case 0x15:
    CPU_REGISTERS_Table[2](cpu, screen, stack, RAM);
    break;
  case 0x18:
    CPU_REGISTERS_Table[3](cpu, screen, stack, RAM);
    break;
  case 0x1E:
    CPU_REGISTERS_Table[4](cpu, screen, stack, RAM);
    break;
  case 0x29:
    CPU_REGISTERS_Table[5](cpu, screen, stack, RAM);
    break;
  case 0x33:
    CPU_REGISTERS_Table[6](cpu, screen, stack, RAM);
    break;
  case 0x55:
    CPU_REGISTERS_Table[7](cpu, screen, stack, RAM);
    break;
  case 0x65:
    CPU_REGISTERS_Table[8](cpu, screen, stack, RAM);
    break;
  default:
    fprintf(stderr, "unknown instruction: %x", cpu->instruction);
  }
}

void (*Chip8_Table[16])(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) = {
    OP_0,    OP_1NNN, OP_2NNN,        OP_3XNN,      OP_4XNN, OP_5XY0,
    OP_6XNN, OP_7XNN, CPU_ARITHMETIC, OP_9XY0,      OP_ANNN, OP_BNNN,
    OP_CXNN, OP_DXYN, KEY_PRESSED,    CPU_REGISTERS};

void (*OP_0_Table[2])(CPU *cpu, SCREEN *screen, STACK *stack,
                      BYTE *RAM) = {OP_00E0, OP_00EE};

void (*CPU_ARITHMETIC_Table[9])(CPU *cpu, SCREEN *screen, STACK *stack,
                                BYTE *RAM) = {OP_8XY0, OP_8XY1, OP_8XY2,
                                              OP_8XY3, OP_8XY4, OP_8XY5,
                                              OP_8XY6, OP_8XY7, OP_8XYE};

void (*KEY_PRESSED_Table[2])(CPU *cpu, SCREEN *screen, STACK *stack,
                             BYTE *RAM) = {OP_EX9E, OP_EXA1};

void (*CPU_REGISTERS_Table[9])(CPU *cpu, SCREEN *screen, STACK *stack,
                               BYTE *RAM) = {OP_FX07, OP_FX0A, OP_FX15,
                                             OP_FX18, OP_FX1E, OP_FX29,
                                             OP_FX33, OP_FX55, OP_FX65};

void decode(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM) {
  cpu->instruction = (RAM[cpu->PC] << 8) + RAM[cpu->PC + 1];
  cpu->PC += 2;

  // TODO: decode and execute within the switch statement
  Chip8_Table[(cpu->instruction & 0xF000) >> 12](cpu, screen, stack, RAM);
}
