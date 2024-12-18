#include "test.h"

#include "hashtable.h"

TEST("Hash table creation", {
  struct HT *hash_table = HT_new();
  AFTER(HT_free(hash_table, 1));
  ASSERT(hash_table != NULL);
});
