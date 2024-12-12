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
 * If ASSERT's content is 0 a message like:
 *
 * "assertion error (__FILE__:__LINE__): y == 2"
 * 
 * should be displayed in stderr.
 */

#define ASSERT(c) \
  do { \
    if (!(c)) { \
      fprintf(stderr, \
              "assertion error (%s:%i): %s", \
              __FILE__ , \
              __LINE__ , \
              #c); \
    } \
  } while(0)

#endif // RESTIN_TEST_H
