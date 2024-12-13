#include "json.h"

#include "linkedlist.h"
#include "hashtable.h"
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
        struct LL *array; // of struct JSONNode
        struct HT *object; // of struct JSONNode
    } value;
};

struct JSON __restin_json_null;
struct JSON __restin_json_true;
struct JSON __restin_json_false;

struct JSON *JSON_null() {
  return &__restin_json_null;
}

struct JSON *JSON_true() {
  return &__restin_json_true;
}

struct JSON *JSON_false() {
  return &__restin_json_false;
}

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
