#include "datastructure.h"
#include <stdlib.h>
#include <string.h>


// Static Stack {{{
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
// }}} END Static Stack

// Linked List {{{
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
// }}} END Linked List

// Hash Table {{{
#define HASH_TABLE_BUCKET_SIZE (1 << 8)
#define HASH_TABLE_KEY_MAX_SIZE (1 << 8)
struct HT_entry {
  char key[HASH_TABLE_KEY_MAX_SIZE];
  void *value;
};
struct HT {
  struct LL *bucket[HASH_TABLE_BUCKET_SIZE];
};

// WARNING: this is a hacky way to implement the closure function for finding
// an entry by key. Not thread-safe.
static char _HT_key[HASH_TABLE_KEY_MAX_SIZE];
int _HT_key_filterfunc(void *entry) {
  return strcmp(((struct HT_entry *)entry)->key, _HT_key) == 0;
}

LL_filtfunc _HT_entry_find_by_key_filtfunc(const char *key) {
  strcpy(_HT_key, key);
  return _HT_key_filterfunc;
}
// END WARNING

int HT_hash(const char *key) {
  int hash = 0;
  for (int i = 0; key[i]; i++) {
    hash = (hash << 5) + key[i];
  }
  hash = abs(hash);
  return hash % HASH_TABLE_BUCKET_SIZE;
}

void HT_init(struct HT *hash_table) {
  for (int i = 0; i < HASH_TABLE_BUCKET_SIZE; i++) {
    hash_table->bucket[i] = NULL;
  }
}

void *HT_get(struct HT *hash_table, const char *key) {
  int hash = HT_hash(key);
  struct LL *bucket = hash_table->bucket[hash],
            *entry = LL_find(bucket, _HT_entry_find_by_key_filtfunc(key), NULL);
  if (entry) {
    return ((struct HT_entry *)entry->value)->value;
  }
  return NULL;
}

void HT_set(struct HT *hash_table, const char *key, void *data) {
  int hash = HT_hash(key);
  struct LL *ll_bucket = hash_table->bucket[hash], *ll_prev, *ll_entry;
  struct HT_entry *entry;
  if (!ll_bucket) {
    entry = malloc(sizeof(struct HT_entry));
    strcpy(entry->key, key);
    entry->value = data;
    hash_table->bucket[hash] = LL_new(entry);
  } else if ((ll_entry = LL_find(ll_bucket, _HT_entry_find_by_key_filtfunc(key),
                                 &ll_prev))) {
    entry = ll_entry->value;
    entry->value = data;
  } else {
    entry = malloc(sizeof(struct HT_entry));
    strcpy(entry->key, key);
    entry->value = data;
    LL_insert(ll_prev, data);
  }
}

int HT_keys(struct HT *hash_table, char **keys) {
  int i = 0;
  for (int j = 0; j < HASH_TABLE_BUCKET_SIZE; j++) {
    struct LL *bucket = hash_table->bucket[j];
    while (bucket) {
      struct HT_entry *entry = bucket->value;
      strcpy(keys[i++], entry->key);
      bucket = bucket->next;
    }
  }
  return i;
}

void HT_free(struct HT *hash_table) {
  struct LL *ll_bucket, *ll_iter;
  struct HT_entry *entry;
  for (int i = 0; i < HASH_TABLE_BUCKET_SIZE; i++) {
    ll_bucket = hash_table->bucket[i];
    ll_iter = ll_bucket;
    while (ll_iter) {
      entry = ll_iter->value;
      free(entry);
      ll_iter = ll_iter->next;
    }
    LL_free(ll_bucket);
  }
}

int HT_foreach(struct HT *hash_table, void (*func)(char *, void *)) {
  int i = 0;
  for (int j = 0; j < HASH_TABLE_BUCKET_SIZE; j++) {
    struct LL *bucket = hash_table->bucket[j];
    while (bucket) {
      struct HT_entry *entry = bucket->value;
      if (func != NULL) {
        func(entry->key, entry->value);
      }
      bucket = bucket->next;
      i++;
    }
  }
  return i;
}

// vim: fdm=marker
