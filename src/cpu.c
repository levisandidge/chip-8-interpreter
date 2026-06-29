#include "../include/cpu.h"

void initialize_cpu(CPU *cpu) {
  cpu->PC = PROGRAM_START;
  cpu->SP = 0;
  cpu->I = 0;
  cpu->DELAY_TIMER = 0;
  cpu->SOUND_TIMER = 0;
  cpu->rerender = false;
  memset(cpu->V, 0, sizeof(cpu->V));
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