#ifndef RESTIN_TEST_H
#define RESTIN_TEST_H

#include <stdio.h>
#include <stdlib.h>

/* Expected usage:
 *
 * TEST("Some test scenario", {
 *    int x = 1;
 *    int y = increment(1);
 *    ASSERT(y == 2);
 * });
 *
 * TEST("Dynamic memory allocation", {
 *    int *x = malloc(sizeof(int));
 *    AFTER(free(x));
 *    ASSERT(x != NULL);
 * });
 *
 * If ASSERT's content is 0 a message like:
 *
 * "assertion error (__FILE__:__LINE__): y == 2"
 * 
 * should be displayed in stderr.
 */

#define AFTER(c)
#define BEFORE(c)
#define TEST_START(desc)
#define TEST_END

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

#endif // RESTIN_TEST_H
