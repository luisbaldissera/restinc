#include "test.h"
#include "../main/hashtable.h"

#define MAX_TEST_CASES 100

struct HT *__all_tests = NULL;
int __total_tests = 0;

#if PROFILE == 1
int main(int argc, char **argv) { return TEST_runall(); }
#endif

int TEST_register(char const *casedesc, TESTCase testcase) {
  if (__all_tests == NULL) {
    __all_tests = HT_new();
  }
  __total_tests += HT_set(__all_tests, casedesc, testcase);
  if (__total_tests > MAX_TEST_CASES) {
    HT_free(__all_tests, 1);
    return EMAXTESTS;
  }
  return 0;
}

int TEST_runcase(char const *casedesc) {
  TESTCase testcase = HT_get(__all_tests, casedesc);
  printf("START '%s'\n", casedesc);
  if (testcase == NULL) {
    return ECASENOTFOUND;
  }
  int ret = testcase();
  printf("END '%s' ... %s\n", casedesc, ret == 0 ? "OK" : "FAIL");
  return ret;
}

int TEST_runall() {
  printf("Running all test cases\n");
  char *keys[MAX_TEST_CASES];
  int keysc = HT_keys(__all_tests, keys);
  for (int i = 0; i < keysc; i++) {
    int result = TEST_runcase(keys[i]);
    if (result != 0) {
      return result;
    }
  }
  return 0;
}
