#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

// Static stack {{{
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

// }}}

// Linked List {{{
struct LL;
/**
 * Filter function for linked list find. Its implementation should return
 * non-zero value if the element should be matched.
 */
typedef int (*LL_filtfunc)(void *);
/**
 * Create a new linked list with the given data.
 */
struct LL *LL_new(void *data);
/**
 * Insert a new element after the linked list and returns the new created
 * element.
 */
struct LL *LL_insert(struct LL *linked_list, void *data);
/**
 * Find an element in the linked list that matches the filter function. If prev
 * is not NULL, it will store the previous element from the found element in
 * it. If not found, it will return NULL and prev will be the last element. If
 * the found element is the first element, prev will be NULL.
 */
struct LL *LL_find(struct LL *linked_list, LL_filtfunc filtfunc,
                   struct LL **prev);
/**
 * Free the linked list. Note that the data is not freed.
 */
void LL_free(struct LL *linked_list);
/**
 * Write the data from the linked list as an array to the given array. Returns
 * the number of elements written.
 */
int LL_array(struct LL *linked_list, void **array);
/**
 * Iterate through the linked list and call the given function for each
 * element and returns the total number of elements iterated.
 */
int LL_foreach(struct LL *ll, void (*func)(void *));

// }}}

// Hash table {{{
#define HASH_TABLE_BUCKET_SIZE (1 << 8)
#define HASH_TABLE_KEY_MAX_SIZE (1 << 8)
struct HT;
/**
 * Hash function for the hash table.
 */
int HT_hash(const char *key);
/**
 * Initialize the hash table.
 */
void HT_init(struct HT *hash_table);
/**
 * Set a key in the hash table with the given data.
 */
void HT_set(struct HT *hash_table, const char *key, void *data);
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
 * Free the hash table. Note that the data is not freed.
 */
void HT_free(struct HT *hash_table);
/**
 * Iterate through the hash table and call the given function for each element.
 */
int HT_foreach(struct HT *hash_table, void (*func)(char *, void *));
// }}}

#endif

// vim: fdm=marker
