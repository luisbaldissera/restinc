#ifndef RESTIN_JSON_H
#define RESTIN_JSON_H

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

void JSON_free(struct JSON *node);

void JSON_object_set(struct JSON *object, const char *key, struct JSON *value);
struct JSON *JSON_object_get(const struct JSON *object, const char *key);

#endif // RESTIN_JSON_H
