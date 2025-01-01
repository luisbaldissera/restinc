#ifndef RESTIN_HASHTABLE_H
#define RESTIN_HASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

struct HT;

/**
 * Creates a new hash table.
 */
struct HT *HT_new();
/**
 * Hash function for the hash table.
 */
int HT_hash(const char *key);
/**
 * Initialize the hash table.
 */
void HT_init(struct HT *hash_table);
/**
 * Set a key in the hash table with the given data. Returns 1 if the key is new and 0 if it is updated.
 */
int HT_set(struct HT *hash_table, const char *key, void *data);
/**
 * Get all the keys from the hash table and store them in the given array.
 * Returns the number of keys.
 */
int HT_keys(struct HT *hash_table, char **keys);
/**
 * Get the data from the hash table with the given key.
 */
void *HT_get(struct HT *hash_table, const char *key);
/**
 * Free the hash table indexes. Note that the data is not freed. if self is
 * non-zero, the hash table itself is freed.
 */
void HT_free(struct HT *hash_table, int self);
/**
 * Iterate through the hash table and call the given function for each element.
 */
int HT_foreach(struct HT *hash_table, void (*func)(char *, void *));

#ifdef __cplusplus
}
#endif

#endif // RESTIN_HASHTABLE_H
