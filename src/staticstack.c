#include "staticstack.h"

#define STATIC_STACK_MAX_SIZE (1 << 16)

struct SS {
  void *data[STATIC_STACK_MAX_SIZE];
  int size;
};

void SS_init(struct SS *stack) { stack->size = 0; }

void SS_push(struct SS *stack, void *data) {
  stack->data[stack->size++] = data;
}

void *SS_pop(struct SS *stack) { return stack->data[--stack->size]; }

void *SS_peek(struct SS *stack) { return stack->data[stack->size - 1]; }

int SS_size(struct SS *stack) { return stack->size; }
