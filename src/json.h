#ifndef RESTIN_JSON_H
#define RESTIN_JSON_H

#include <stdio.h>

struct JSON;

struct JSON *JSON_null();
struct JSON *JSON_true();
struct JSON *JSON_false();
struct JSON *JSON_decimal(double value);
struct JSON *JSON_integer(int value);
struct JSON *JSON_string(const char *value);
struct JSON *JSON_array();
struct JSON *JSON_object();

int JSON_isnull(struct JSON *node);
int JSON_isboolean(struct JSON *node);
int JSON_isnumber(struct JSON *node);
int JSON_isstring(struct JSON *node);
int JSON_isarray(struct JSON *node);
int JSON_isobject(struct JSON *node);

int JSON_cmp(struct JSON *a, struct JSON *b);
int JSON_fwrite(struct JSON *node, FILE *out);

void JSON_free(struct JSON *node);

void JSON_object_set(struct JSON *object, const char *key, struct JSON *value);
struct JSON *JSON_object_get(const struct JSON *object, const char *key);

void JSON_array_push(struct JSON *array, struct JSON *value);

#define JSON_OBJ(...)
#define JSON_ARR(...)
#define JSON_STR(s)
#define JSON_INT(i)
#define JSON_DEC(d)
#define JSON_TRU
#define JSON_FAL
#define JSON_NUL

#endif // RESTIN_JSON_H
