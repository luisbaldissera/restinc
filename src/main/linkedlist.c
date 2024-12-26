#include "linkedlist.h"
#include <stdlib.h>

struct LL {
  void *value;
  struct LL *next;
};

struct LL *LL_new(void *data) {
  struct LL *linked_list = malloc(sizeof(struct LL));
  linked_list->value = data;
  linked_list->next = NULL;
  return linked_list;
}

struct LL *LL_insert(struct LL *linked_list, void *data) {
  struct LL *new = malloc(sizeof(struct LL));
  new->value = data;
  new->next = linked_list->next;
  linked_list->next = new;
  return new;
}

void LL_free(struct LL *ll) {
  struct LL *next;
  while (ll) {
    next = ll->next;
    free(ll);
    ll = next;
  }
}

struct LL *LL_find(struct LL *ll, LL_filtfunc filtfunc, struct LL **prev) {
  while (ll) {
    if (filtfunc(ll->value)) {
      return ll;
    }
    if (prev) {
      *prev = ll;
    }
    ll = ll->next;
  }
  return NULL;
}

int LL_array(struct LL *ll, void **array) {
  int i = 0;
  while (ll) {
    array[i++] = ll->value;
    ll = ll->next;
  }
  return i;
}

int LL_foreach(struct LL *ll, void (*func)(void *)) {
  int i = 0;
  while (ll) {
    if (func != NULL) {
      func(ll->value);
    }
    ll = ll->next;
    i++;
  }
  return i;
}

struct LL *LL_next(struct LL *ll) { return ll->next; }

void *LL_value(struct LL *ll) { return ll->value; }

