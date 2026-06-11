#pragma once

#include <stdio.h>

#include "cpu.h"
#include "instructions.h"

void (*Chip8_Table[16])(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);
void (*OP_0_Table[2])(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);
void (*CPU_ARITHMETIC_Table[9])(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);
void (*KEY_PRESSED_Table[2])(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);
void (*CPU_REGISTERS_Table[9])(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void OP_0(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void CPU_ARITHMETIC(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void KEY_PRESSED(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void CPU_REGISTERS(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);

void decode(CPU *cpu, SCREEN *screen, STACK *stack, BYTE *RAM);