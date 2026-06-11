#pragma once

#include <stdlib.h>

#include "cpu.h"

void OP_00E0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_00EE(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_1NNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_2NNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_3XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_4XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_5XY0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_6XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_7XNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY1(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY2(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY3(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY4(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY5(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY6(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XY7(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_8XYE(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_9XY0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_ANNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_BNNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_NNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_CXNN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_DXYN(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_EX9E(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_EXA1(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX07(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX0A(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX15(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX18(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX1E(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX29(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX33(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX55(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_FX65(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);
