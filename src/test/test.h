#ifndef RESTIN_TEST_H
#define RESTIN_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

// Testing library.
// TODO: maybe using https://github.com/zorgnax/libtap library

#define EMAXTESTS     100
#define ECASENOTFOUND 101
#define ETESTFAIL     102

typedef int (*TESTCase)();

int TEST_register(char const * casedesc, TESTCase testcase);
int TEST_runcase(char const * casedesc);
int TEST_runall();

#ifdef __cplusplus
}
#endif

#endif // RESTIN_TEST_H
