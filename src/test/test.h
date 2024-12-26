#ifndef RESTIN_TEST_H
#define RESTIN_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

// Testing library.
// TODO: maybe using https://github.com/zorgnax/libtap library

#define AFTER(c)
#define BEFORE(c)
#define TEST(d, b)

#define ASSERT(c) \
  do { \
    if (!(c)) { \
      fprintf(stderr, \
              "assertion error (%s:%i): %s", \
              __FILE__ , \
              __LINE__ , \
              #c); \
      return 1; \
    } \
  } while(0)

typedef int (*TESTCase)(void);

int TEST_register(const char * casedesc, TESTCase testcase);
int TEST_runcase(const char * casedesc);
int TEST_runall();

#ifdef __cplusplus
}
#endif

#endif // RESTIN_TEST_H
