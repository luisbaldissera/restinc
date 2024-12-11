#include "hashtable.h"

#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>

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
    return ((struct HT_entry *) LL_value(entry))->value;
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
    entry = LL_value(ll_entry);
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
      struct HT_entry *entry = LL_value(bucket);
      strcpy(keys[i++], entry->key);
      bucket = LL_next(bucket);
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
      entry = LL_value(ll_iter);
      free(entry);
      ll_iter = LL_next(ll_iter);
    }
    LL_free(ll_bucket);
  }
}

int HT_foreach(struct HT *hash_table, void (*func)(char *, void *)) {
  int i = 0;
  for (int j = 0; j < HASH_TABLE_BUCKET_SIZE; j++) {
    struct LL *bucket = hash_table->bucket[j];
    while (bucket) {
      struct HT_entry *entry = LL_value(bucket);
      if (func != NULL) {
        func(entry->key, entry->value);
      }
      bucket = LL_next(bucket);
      i++;
    }
  }
  return i;
}

