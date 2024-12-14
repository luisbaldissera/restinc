#include "json.h"
#include "hashtable.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <string.h>

enum JSONType {
  JSON_DECIMAL,
  JSON_INTEGER,
  JSON_STRING,
  JSON_ARRAY,
  JSON_OBJECT
};

struct JSON {
  enum JSONType type;
  union {
    double decimal;
    int integer;
    char *string;
    int boolean;
    struct {
      struct LL *first;
      struct LL *last;
    } array;
    struct HT *object; // of struct JSONNode
  } value;
};

struct JSON __restin_json_null;
struct JSON __restin_json_true;
struct JSON __restin_json_false;

struct JSON *JSON_null() { return &__restin_json_null; }

struct JSON *JSON_true() { return &__restin_json_true; }

struct JSON *JSON_false() { return &__restin_json_false; }

struct JSON *JSON_decimal(double value) {
  struct JSON *json = malloc(sizeof(struct JSON));
  json->type = JSON_DECIMAL;
  json->value.decimal = value;
  return json;
}

struct JSON *JSON_integer(int value) {
  struct JSON *json = malloc(sizeof(struct JSON));
  json->type = JSON_DECIMAL;
  json->value.integer = value;
  return json;
}

struct JSON *JSON_string(const char *value) {
  struct JSON *json = malloc(sizeof(struct JSON));
  json->type = JSON_STRING;
  return json;
}

struct JSON *JSON_array() {
  struct JSON *json = malloc(sizeof(struct JSON));
  json->type = JSON_ARRAY;
  json->value.array.first = NULL;
  json->value.array.last = NULL;
  return json;
}

struct JSON *JSON_object() {
  struct JSON *json = malloc(sizeof(struct JSON));
  json->type = JSON_OBJECT;
  json->value.object = HT_new();
  return json;
}

int JSON_isnull(struct JSON *node) { return node == &__restin_json_null; }

int JSON_isboolean(struct JSON *node) {
  return node == &__restin_json_true || node == &__restin_json_false;
}

int JSON_isnumber(struct JSON *node) {
  return node->type == JSON_DECIMAL || node->type == JSON_INTEGER;
}

int JSON_isstring(struct JSON *node) { return node->type == JSON_STRING; }

int JSON_isarray(struct JSON *node) { return node->type == JSON_ARRAY; }

int JSON_isobject(struct JSON *node) { return node->type == JSON_OBJECT; }

void JSON_object_set(struct JSON *object, const char *key, struct JSON *value) {
  HT_set(object->value.object, key, value);
}

struct JSON *JSON_object_get(const struct JSON *object, const char *key) {
  return HT_get(object->value.object, key);
}

void JSON_array_push(struct JSON *array, struct JSON *value) {
  if (array->value.array.first == NULL) {
    struct LL *node = LL_new(value);
    array->value.array.first = node;
    array->value.array.last = node;
  } else {
    array->value.array.last = LL_insert(array->value.array.last, value);
  }
}
