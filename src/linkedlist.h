#ifndef RESTIN_LINKEDLIST_H
#define RESTIN_LINKEDLIST_H

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
/**
 * Get the next element in the linked list.
 */
struct LL * LL_next(struct LL *ll);
/**
 * Get the value of the linked list element.
 */
void *LL_value(struct LL *ll);

#endif // RESTIN_LINKEDLIST_H
