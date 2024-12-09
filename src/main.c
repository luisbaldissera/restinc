#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Static stack {{{
#define STATIC_STACK_MAX_SIZE (1 << 16)
struct SS {
  void *data[STATIC_STACK_MAX_SIZE];
  int size;
};
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

void SS_init(struct SS *stack) { stack->size = 0; }

void SS_push(struct SS *stack, void *data) {
  stack->data[stack->size++] = data;
}

void *SS_pop(struct SS *stack) { return stack->data[--stack->size]; }

void *SS_peek(struct SS *stack) { return stack->data[stack->size - 1]; }
// }}}

// Linked List {{{
struct LL {
  void *value;
  struct LL *next;
};
/**
 * Filter function for linked list find. Its implementation should return
 * non-zero value if the element should be matched.
 */
typedef int (*LL_filtfunc)(void *);
/**
 * Iterator for linked list to iterate through the elements.
 */
typedef struct LL **LL_iter;
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
// }}}

// Hash table {{{
#define HASH_TABLE_BUCKET_SIZE (1 << 8)
#define HASH_TABLE_KEY_MAX_SIZE (1 << 8)
struct HT_entry {
  char key[HASH_TABLE_KEY_MAX_SIZE];
  void *value;
};
struct HT {
  struct LL *bucket[HASH_TABLE_BUCKET_SIZE];
};
typedef struct HT_entry *HT_iter;
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
// }}}

// Http {{{
#define HTTP_REQUEST_PATH_MAX_SIZE (1 << 8)

#define HTTP_HEADER_MAX_COUNT (1 << 8)
#define HTTP_HEADER_KEY_MAX_SIZE (1 << 8)
#define HTTP_HEADER_VALUE_MAX_SIZE (1 << 8)

#define HTTP_SERVER_PORT atoi(getenv("HTTP_SERVER_PORT"))
enum HTTP_Method {
  HTTP_METHOD_GET,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT,
  HTTP_METHOD_OPTIONS,
  HTTP_METHOD_PATCH,
  HTTP_METHOD_HEAD,
  HTTP_METHOD_DELETE,
  HTTP_METHOD_CONNECT,
  HTTP_METHOD_TRACE
};

struct HTTP_Header {
  char key[HTTP_HEADER_KEY_MAX_SIZE];
  char value[HTTP_HEADER_VALUE_MAX_SIZE];
};

struct HTTP_Request {
  enum HTTP_Method method;
  char path[HTTP_REQUEST_PATH_MAX_SIZE];
  struct HTTP_Header head[HTTP_HEADER_MAX_COUNT];
  int headc;
  FILE *body;
};

char *HTTP_header(struct HTTP_Request *req, const char *key,
                  const char *defval);

struct HTTP_Response {
  int status;
  struct HTTP_Header head[HTTP_HEADER_MAX_COUNT];
  int headc;
  FILE *body;
};

void HTTP_handle(struct HTTP_Request *req, struct HTTP_Response *resp);

// }}}

// Server {{{
static volatile sig_atomic_t stop_server = 0;
void SIGINT_handler(int _);
void handle_request(FILE *req, FILE *resp);

int main() {
  stop_server = 0;
  signal(SIGINT, SIGINT_handler);
  int err, server_sck, session_sck, read_size;
  struct sockaddr_in addr;
  char method[16], path[256], proto[16];
  socklen_t addrlen;
  if ((server_sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failure");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(HTTP_SERVER_PORT);
  addrlen = sizeof(addr);
  if ((err = bind(server_sck, (const struct sockaddr *)&addr, sizeof(addr))) !=
      EXIT_SUCCESS) {
    perror("socket bind failure");
    exit(EXIT_FAILURE);
  }
  if ((err = listen(server_sck, 0)) != EXIT_SUCCESS) {
    perror("socket listen failure");
    exit(EXIT_FAILURE);
  }
  printf("Server started on port %i...\n", HTTP_SERVER_PORT);
  while (!stop_server &&
         (session_sck =
              accept(server_sck, (struct sockaddr *)&addr, &addrlen)) >= 0) {
    printf("handling request\n");
    FILE *req = fdopen(session_sck, "r");
    FILE *resp = fdopen(session_sck, "a");
    handle_request(req, resp);
    fflush(resp);
    fclose(req);
    fclose(resp);
    close(session_sck);
  }
  perror("server exited");
  close(server_sck);
}

void handle_request(FILE *req, FILE *resp) {}

void SIGINT_handler(int _) {
  if (!stop_server) {
    printf("graceful shutdown\n");
    stop_server = 1;
  } else {
    exit(EXIT_FAILURE);
  }
}

// }}}

// vim: fdm=marker
