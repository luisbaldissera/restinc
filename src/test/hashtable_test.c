#include "../main/hashtable.h"
#include "test.h"
#include <assert.h>

int test_HT_new() {
  struct HT *hash_table = HT_new();
  assert(hash_table != NULL);
  HT_free(hash_table, 1);
  return 0;
}
