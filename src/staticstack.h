#ifndef RESTIN_STATICSTACK_H
#define RESTIN_STATICSTACK_H

struct SS;
/**
 * Initialize the static stack.
 */
void SS_init(struct SS *stack);
/**
 * Push data to the static stack.
 */
void SS_push(struct SS *stack, void *data);
/**
 * Pop data from the static stack.
 */
void *SS_pop(struct SS *stack);
/**
 * Peek the top element of the static stack, without removing it.
 */
void *SS_peek(struct SS *stack);
/**
 * Get the size of the static stack.
 */
int SS_size(struct SS *stack);

#endif // RESTIN_STATICSTACK_H
